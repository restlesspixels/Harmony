# Importing Splats

The plugin supports Gaussian and BB Splat source data. All formats import into the same asset type, `UHarmonyDataAsset`.

---

## How to Import

1. Open the **Content Browser** in Unreal Editor.
2. Drag a supported splat file into the Content Browser, or use **Import** from the toolbar.
3. The importer detects the format from the file extension.
4. A `UHarmonyDataAsset` is created in the current folder.

---

## Supported Formats

| Extension | Format | Notes |
|---|---|---|
| `.ply` | Gaussian or BB Splat PLY | A BB Splat PLY is detected when `texture_alpha.npz` and `texture_color.npz` are beside it |
| `.spz` | Niantic SPZ | Quantized source format |
| `.sog` | PlayCanvas SOG | Lossy source format based on packed image data |
| `.splat` | Packed binary splat | DC colour only, no higher-order SH |
| `.ksplat` | GaussianSplats3D KSPLAT | Viewer-native binary cache format |
| `.npz` | NumPy archive | May use sparse geometry/feature indexing |

---

## Compression On Import

Import file format support and compression are separate concerns.

Import-time compression is controlled by:

`Edit -> Project Settings -> Plugins -> Harmony Import -> Import Compressed`

`Import Compressed` is enabled by default.

Behavior:

- Gaussian `.ply`, `.spz`, `.sog`, `.splat`, and `.ksplat` import as Expanded data first, then auto-compress before save if `Import Compressed` is enabled
- BB Splat `.ply` imports remain Expanded because the Gaussian covariance compression format cannot represent an oriented UV plane and its texture
- `.npz` imports directly as Compressed data

If `Import Compressed` is disabled, expanded-source formats stay Expanded on import.

### BB Splat `.ply`

Import the PLY from an exported BB Splat checkpoint folder. Harmony expects these files in the same directory:

- the `.ply` selected for import
- `texture_alpha.npz`, containing `texture_alpha.npy` with shape `[N, 16, 16]`
- `texture_color.npz`, containing `texture_color.npy` with shape `[N, 3, 16, 16]`

The importer reconstructs the residual alpha representation used by BB Splat and stores directly sampleable RGBA8 texels in the asset. See [BB Splat](07_BBSplat.md) for runtime behavior and limitations.

### Notes on `.npz`

`NPZ` files arrive pre-quantized and are imported directly into the compressed layout.

- They may use **sparse indexing**, where geometry or feature arrays are shared across fewer unique entries than total points.
- Sparse compressed assets cannot be expanded back into a standard Expanded asset through the normal editor workflow. Re-import from the original source file if you need an Expanded version.

---

## After Import

Once imported, the asset appears in the Content Browser with a thumbnail preview. You can:

- Drag it into the level to place it as an `AHarmonyActor`
- Right-click it to **Convert to Compressed** if it is currently Expanded. See [Compression](05_Compression.md)
- Hover over it in the Content Browser to see point count, encoding, and estimated disk size

If you need Expanded imports for testing or inspection, disable `Import Compressed` in **Project Settings** before importing.

---

## Adding New Formats

Each format is handled by a dedicated importer class in `Source/HarmonyEditor/Private/Import/`:

| File | Format |
|---|---|
| `HarmonyPlyImporter.cpp` | `.ply` |
| `HarmonyBBSplatImporter.cpp` | BB Splat `.ply` with companion texture archives |
| `HarmonySpzImporter.cpp` | `.spz` |
| `HarmonySogImporter.cpp` | `.sog` |
| `HarmonySplatImporter.cpp` | `.splat` |
| `HarmonyKsplatImporter.cpp` | `.ksplat` |
| `HarmonyNpzImporter.cpp` | `.npz` |

Each importer fills a `FHarmonyRawData` struct. `UHarmonyPlyFactory` dispatches to the correct importer based on file extension, then applies the current import settings before saving the final `UHarmonyDataAsset`.
