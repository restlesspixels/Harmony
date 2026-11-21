# .harmony Files

A `.harmony` file holds a whole Harmony asset in one file that does not depend on the engine version. Use it to move assets between Unreal Engine versions (a `.uasset` saved by a newer engine cannot be opened by an older one), between projects, or into tools outside Unreal.

---

## What a .harmony file carries

Unlike a PLY export, nothing about the splat is lost:

- the splat data byte for byte, in the asset's own encoding (a Compressed asset stays compressed and is not re-encoded)
- the variant sets with all their option data
- the lighting probes (the asset's own and each variant option's): the HDR image and the sun, exposure and radiance data
- the shadow proxy (ground height map) and the reflection proxy (floor material)
- optionally, a placed actor's transform, label and component settings: near fade, depth offset, opacity and splat scale, active variants, and the lighting, shadow catcher and reflection settings

What does not travel:

| Not carried | Why |
|---|---|
| Fallback meshes | **Fallback Tiers** are written as asset paths only. The static meshes, materials and textures are separate assets; move them yourself. |
| **Transition Duration** | The variant fade time on the Harmony component is not written. It returns to its default (0.5 s) on import. |
| Shadow catcher **Casters** | The listed actors belong to the level. The **Caster Tag** is carried. |
| Project settings | Composition, tonemapping and the other [rendering settings](04_RenderingSettings.md) belong to the project. |

The importing project needs a Harmony release at least as new as the one that wrote the file. Nothing else from the exporting project is needed.

---

## Exporting

| From | How | Writes |
|---|---|---|
| Content Browser | Right-click one or more Harmony Data Assets > **Export to .harmony...** | The asset |
| Level | Right-click one or more Harmony Actors > **Export to .harmony...** (in the menu's **Harmony** section) | The asset and the actor's settings |
| Python | `unreal.HarmonyFileLibrary.export_harmony_asset(asset, path)` or `export_harmony_actor(actor, path)` | As above |

With one item selected you are asked for a file name. With several you are asked for a folder, and each item is written as `<Name>.harmony`. A notification with an **Open folder** link appears when the export finishes.

Export from the **level** when you want the splat to arrive already positioned and configured. Export from the **Content Browser** when you only want the data.

## Importing

| Into | How |
|---|---|
| Content Browser (asset only) | Drag the `.harmony` file in, or use **Import**. |
| Level (asset and actor) | **Tools > Harmony > Import .harmony into Level...** Pick the file, then choose where to save the asset. A Harmony Actor is placed with the file's transform and settings. If the file was exported from the Content Browser and has no actor, the new actor is placed at the origin with default settings. |
| Python | `unreal.HarmonyFileLibrary.import_harmony_file(path, "/Game/Splats/Scene", save)` or `import_harmony_file_into_level(path, "/Game/Splats/Scene", save)` |

**Save afterwards.** **Import .harmony into Level...** creates the asset but does not save it to disk. Use **File > Save All** (or save the asset and the level) before closing the editor, or the import is lost. In Python, pass `True` for `save` to save the asset as part of the import.

The asset path you choose must not already exist. Importing never overwrites an asset.

In Python a successful export returns `""` (the error string) and a failed one returns `None`. An import returns `(asset, error)` or `(actor, error)`.

Fields in the file that the importing version of Harmony does not know are skipped with a warning in the Output Log (category `LogHarmonyFile`). Fields the importing version has that the file lacks keep their defaults.

---

## Good to know

- **Files are large.** A `.harmony` file is roughly the size of the asset's data. Chunks are zlib-compressed where that makes them smaller.
- **It is an interchange format, not a runtime format.** Packaged games load ordinary cooked assets. `.harmony` import and export are editor features.
- **Engine version.** The file itself is engine-independent, but you still need a Harmony release built for the engine you are importing into.

---

## Format (version 1)

This section is for people writing their own reader. All integers are little endian. The layout resembles glTF's GLB: a JSON manifest plus binary chunks.

**Header** (32 bytes): `"HRMY"`, `uint32` format version, `uint32` chunk count, `uint32` flags (0), `uint64` chunk table offset, `uint64` reserved.

**Chunk table** (40 bytes per chunk): `char[4]` type (`JSON`, `BUFF`, `IMG `), `uint32` codec (0 stored, 1 zlib blocks), `uint64` offset, `uint64` stored size, `uint64` raw size, `uint32` CRC-32 of the raw bytes (zlib's `crc32`), `uint32` reserved.

**zlib blocks**: `uint32` block size B, `uint32` block count N, N × `uint32` stored block sizes, then the blocks. Block *i* is raw bytes `[i·B, min((i+1)·B, raw size))`. A block whose stored size equals its raw size is stored as is; any other is one zlib stream (RFC 1950). Blocks are 8 MiB.

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

`properties` are the asset's saved properties by their C++ names (see the [Asset reference](06_AssetReference.md#property-names-for-tools)): numbers, bools, strings (names, strings, enums by value name; maps, sets and values JSON cannot hold, such as NaN, as Unreal export text), objects (structs) and arrays, except:

- An array of numbers, or of structs made of numbers of one type, is `{ "type": "f32x4", "count": N, "buffer": <chunk> }`: the N elements packed as in memory. Scalar types are `u8 i8 u16 i16 u32 i32 u64 i64 f32 f64`; a struct of n is `<scalar>x<n>` (`FVector4f` is `f32x4`).
- A lighting probe's cubemap is `{ "image": <chunk>, "name", "width", "height", "slices", "format": "BGRE8", "compressionSettings", "lodGroup", "srgb" }`: the texture's source pixels, 4 bytes (RGBE) each. A long-lat probe is one slice of width × width/2.

The format version only goes up for changes an older reader cannot handle; readers refuse newer versions. New fields need no new version.

### Reference reader

[`harmony_file_reader.py`](harmony_file_reader.py), next to this page, is a reference reader in plain Python:

```bash
python harmony_file_reader.py Scene.harmony                  # summary
python harmony_file_reader.py Scene.harmony --manifest       # the manifest
python harmony_file_reader.py A.harmony --compare B.harmony  # same contents?
```

---

## Next steps

- [Asset reference](06_AssetReference.md): what the properties mean
- [Importing splats](02_Importing.md): other import formats
