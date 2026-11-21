# .harmony Files

A `.harmony` file holds a whole Harmony asset in one file that does not depend on the engine version. Use it to move assets
between Unreal Engine versions (a package saved by a newer engine cannot be opened by an older one), between projects, or
into tools outside Unreal.

Unlike a PLY export, nothing is lost:

- the splat buffers byte for byte, in the asset's own encoding (a Compressed asset stays compressed and is not re-encoded)
- the variant sets with all their option data
- the lighting probes (the asset's own and each variant option's): the HDR image and the sun, exposure and radiance data
- the shadow proxy (ground heightfield) and the reflection proxy (floor material)
- optionally a placed actor's transform, label and component settings: near fade, depth offset, opacity and splat scale,
  active variants, lighting and shadow-catcher / reflection settings

The importing side needs a Harmony plugin built from this source (or newer). Nothing else from the exporting project is
needed.

---

## Exporting

| From | How | Writes |
|---|---|---|
| Content Browser | right-click Harmony asset(s) > **Export to .harmony...** | the asset |
| Level | right-click Harmony actor(s) > **Harmony > Export to .harmony...** | the asset + the actor's settings |
| Python | `unreal.HarmonyFileLibrary.export_harmony_asset(asset, path)` / `export_harmony_actor(actor, path)` | as above |

One selected item asks for a file name; several ask for a folder and write `<Name>.harmony` each.

## Importing

| Into | How |
|---|---|
| Content Browser (asset only) | drag the `.harmony` file in, or **Import** |
| Level (asset + actor) | **Tools > Harmony > Import .harmony into Level...**: pick the file, then where to save the asset; an actor is placed with the file's transform and settings (at the origin with defaults when the file has no actor) |
| Python | `unreal.HarmonyFileLibrary.import_harmony_file(path, "/Game/Splats/Scene", save)` / `import_harmony_file_into_level(...)` |

In Python a successful export returns `""` (the error string) and a failed one `None`; an import returns `(asset, error)`.

Fields the file has that the importing build does not know are skipped with a warning in the Output Log (`LogHarmonyFile`);
fields the importing build has that the file lacks keep their defaults. Shadow-catcher caster actors are not carried
(they belong to the level); the caster tag is.

---

## Format (version 1)

All integers little endian. Layout like glTF's GLB: a JSON manifest plus binary chunks.

**Header** (32 bytes): `"HRMY"`, `uint32` format version, `uint32` chunk count, `uint32` flags (0), `uint64` chunk table
offset, `uint64` reserved.

**Chunk table** (40 bytes per chunk): `char[4]` type (`JSON`, `BUFF`, `IMG `), `uint32` codec (0 stored, 1 zlib blocks),
`uint64` offset, `uint64` stored size, `uint64` raw size, `uint32` CRC-32 of the raw bytes (zlib's `crc32`), `uint32`
reserved.

**zlib blocks**: `uint32` block size B, `uint32` block count N, N × `uint32` stored block sizes, then the blocks. Block *i*
is raw bytes `[i·B, min((i+1)·B, raw size))`; a block whose stored size equals its raw size is stored as is, any other is
one zlib stream (RFC 1950). Blocks are 8 MiB and compressed in parallel.

**Manifest** (the one `JSON` chunk, UTF-8):

```json
{
  "format": "harmony", "formatVersion": 1,
  "generator": { "engine": "5.9.0-...", "plugin": "Harmony" },
  "asset": { "name": "...", "path": "...", "class": "HarmonyDataAsset", "properties": { ... } },
  "actor": { "label": "...", "hiddenInGame": false,
             "transform": { "location": [x, y, z], "rotation": [x, y, z, w], "scale": [x, y, z] },
             "components": { "GaussianSplatComponent": { ... }, "LightingComponent": { ... }, "ShadowCatcherComponent": { ... } } }
}
```

`properties` are the asset's saved properties by their C++ names (see [Asset Reference](06_AssetReference.md)): numbers,
bools, strings (names, strings, enums by value name; maps, sets and values JSON cannot hold, such as NaN, as Unreal export
text), objects (structs) and arrays, except:

- an array of numbers, or of structs made of numbers of one type, is `{ "type": "f32x4", "count": N, "buffer": <chunk> }`:
  the N elements packed as in memory. Scalar types `u8 i8 u16 i16 u32 i32 u64 i64 f32 f64`, `<scalar>x<n>` for a struct
  of n (`FVector4f` is `f32x4`).
- a lighting probe's cubemap is `{ "image": <chunk>, "name", "width", "height", "slices", "format": "BGRE8",
  "compressionSettings", "lodGroup", "srgb" }`: the texture's source pixels, 4 bytes (RGBE) each; a long-lat probe is one
  slice of width × width/2.

The format version only goes up for changes an older reader cannot handle; readers refuse newer versions. New fields need
no new version.

`harmony_file_reader.py` (next to this page) is a reference reader in plain Python:

```bash
python harmony_file_reader.py Scene.harmony                  # summary
python harmony_file_reader.py Scene.harmony --manifest       # the manifest
python harmony_file_reader.py A.harmony --compare B.harmony  # same contents?
```
