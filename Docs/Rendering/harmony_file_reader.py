#!/usr/bin/env python3
"""Reference reader for .harmony files (Docs/08_HarmonyFile.md), plain Python 3, no Unreal.

    python harmony_file_reader.py Scene.harmony                 # summary
    python harmony_file_reader.py Scene.harmony --manifest      # the JSON manifest
    python harmony_file_reader.py A.harmony --compare B.harmony # same contents? (names, paths and the writer ignored)

Library use:
    f = HarmonyFile("Scene.harmony")
    props = f.manifest["asset"]["properties"]
    pos = f.array(props["SplatData"]["PosRadius"])   # bytes, little endian, f32 x 4 per splat
"""
import json
import struct
import sys
import zlib

MAGIC = b"HRMY"
SUPPORTED_VERSION = 1
CODEC_STORED, CODEC_ZLIB_BLOCKS = 0, 1
SCALAR_SIZES = {"u8": 1, "i8": 1, "u16": 2, "i16": 2, "u32": 4, "i32": 4, "u64": 8, "i64": 8, "f32": 4, "f64": 8}


def element_size(type_code):
    scalar, _, count = type_code.partition("x")
    return SCALAR_SIZES[scalar] * (int(count) if count else 1)


class HarmonyFile:
    def __init__(self, path):
        self.path = path
        self._file = open(path, "rb")
        magic, self.version, count, _flags, table, _reserved = struct.unpack("<4sIIIQQ", self._file.read(32))
        if magic != MAGIC:
            raise ValueError(f"{path}: not a .harmony file")
        if not 1 <= self.version <= SUPPORTED_VERSION:
            raise ValueError(f"{path}: format version {self.version} (this reader: {SUPPORTED_VERSION})")
        self._file.seek(table)
        self.chunks = []
        for _ in range(count):
            ctype, codec, offset, stored, raw, crc, _ = struct.unpack("<4sIQQQII", self._file.read(40))
            self.chunks.append({"type": ctype.decode("ascii"), "codec": codec, "offset": offset, "stored": stored,
                                "raw": raw, "crc": crc})
        json_index = next(i for i, c in enumerate(self.chunks) if c["type"] == "JSON")
        self.manifest = json.loads(self.chunk(json_index).decode("utf-8"))

    def chunk(self, index):
        c = self.chunks[index]
        self._file.seek(c["offset"])
        data = self._file.read(c["stored"])
        if c["codec"] == CODEC_ZLIB_BLOCKS:
            block_size, n = struct.unpack_from("<II", data, 0)
            sizes = struct.unpack_from(f"<{n}I", data, 8)
            pos, out = 8 + 4 * n, bytearray()
            for i, size in enumerate(sizes):
                raw = min(block_size, c["raw"] - i * block_size)
                block = data[pos:pos + size]
                out += block if size == raw else zlib.decompress(block)
                pos += size
            data = bytes(out)
        elif c["codec"] != CODEC_STORED:
            raise ValueError(f"chunk {index}: unknown codec {c['codec']}")
        if len(data) != c["raw"] or zlib.crc32(data) != c["crc"]:
            raise ValueError(f"chunk {index}: damaged")
        return data

    def array(self, ref):
        """The bytes of an array reference {"type", "count", "buffer"}."""
        if ref["count"] == 0:
            return b""
        data = self.chunk(ref["buffer"])
        assert len(data) == ref["count"] * element_size(ref["type"])
        return data

    def image(self, ref):
        """The RGBE pixels of an image reference {"image", "width", "height", "slices", "format": "BGRE8"}."""
        return self.chunk(ref["image"])


def _walk(value, visit):
    if isinstance(value, dict):
        visit(value)
        for v in value.values():
            _walk(v, visit)
    elif isinstance(value, list):
        for v in value:
            _walk(v, visit)


def summary(f):
    m = f.manifest
    a = m["asset"]
    p = a["properties"]
    sd = p.get("SplatData", {})
    arrays, images, total = 0, 0, 0

    def visit(d):
        nonlocal arrays, images, total
        if "buffer" in d and "type" in d:
            arrays += 1
            total += f.chunks[d["buffer"]]["raw"]
        if "image" in d:
            images += 1
    _walk(p, visit)
    stored = sum(c["stored"] for c in f.chunks)
    print(f"{f.path}: format {f.version}, written by {m['generator'].get('plugin')} on Unreal Engine {m['generator'].get('engine')}")
    print(f"  asset {a['name']} ({a['path']}): {sd.get('NumPoints')} splats, {sd.get('Encoding')}, {sd.get('Representation')}, "
          f"SH degree {sd.get('SHDegree')}")
    print(f"  {arrays} arrays ({total / 2**20:.1f} MiB), {images} image(s), {len(p.get('VariantSets', []))} variant set(s), "
          f"{len(f.chunks)} chunks, {stored / 2**20:.1f} MiB stored")
    if "actor" in m:
        print(f"  actor '{m['actor'].get('label')}' with components {', '.join(m['actor'].get('components', {}))}")


def normalized(f):
    """The manifest with chunk indices replaced by the chunks' sizes and checksums, and names and paths removed."""
    m = json.loads(json.dumps(f.manifest))
    m.pop("generator", None)
    m["asset"].pop("name", None)
    m["asset"].pop("path", None)

    def visit(d):
        for key in ("buffer", "image"):
            if key in d:
                c = f.chunks[d[key]]
                d[key] = f"{c['raw']}:{c['crc']:08x}"
        d.pop("name", None) if "image" in d else None
    _walk(m, visit)
    return m


def main(argv):
    if len(argv) < 2:
        print(__doc__)
        return 2
    f = HarmonyFile(argv[1])
    if "--manifest" in argv:
        print(json.dumps(f.manifest, indent=2))
        return 0
    summary(f)
    if "--compare" in argv:
        g = HarmonyFile(argv[argv.index("--compare") + 1])
        summary(g)
        a, b = normalized(f), normalized(g)
        if a == b:
            print("SAME contents")
            return 0
        for key in sorted(set(a["asset"]["properties"]) | set(b["asset"]["properties"])):
            if a["asset"]["properties"].get(key) != b["asset"]["properties"].get(key):
                print(f"  differs: asset.properties.{key}")
        if a.get("actor") != b.get("actor"):
            print("  differs: actor")
        print("DIFFERENT contents")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
