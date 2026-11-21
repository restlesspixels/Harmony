# Asset Reference

A **Harmony Data Asset** (`UHarmonyDataAsset` in C++) holds everything about one splat: the splats themselves and, for splats baked with Harmony, the lighting data, fallback meshes and variant options that go with them. This page describes what you see when you open one, and lists the underlying property names for people writing tools.

---

## In the Content Browser

Harmony Data Assets have no custom thumbnail. Hover over one to see:

| Tooltip field | Meaning |
|---|---|
| **Num Points** | Number of splats |
| **Encoding** | Expanded or Compressed |
| **Representation** | 3D Gaussian or BB Splat |
| **Splat Data Size MB** | Estimated size of the stored splat data |

### Right-click actions

| Action | Shown for | Result |
|---|---|---|
| **Convert to Compressed** | Expanded 3D Gaussian assets | Creates and saves `<Name>_Compressed`. The original is unchanged. |
| **Decimate...** | 3D Gaussian assets | Asks for a percentage to keep, then creates and saves `<Name>_Decimated<NN>`. The original is unchanged. Variant sets are not carried over. |
| **Export to .harmony...** | All | Writes the complete asset to an engine-independent file. See [.harmony files](08_HarmonyFile.md). |
| **Export to PLY...** | 3D Gaussian assets | Asks for a folder, then writes a standard Gaussian splat PLY of the base look, plus one PLY per baked variant option. Lighting data is not included. Hidden when the selection includes a BB Splat asset. |

The first two are covered in [Compression and decimation](05_CompressionAndDecimation.md). All four work on a multiple selection.

---

## In the asset's Details

Double-click the asset. There is no custom editor; the standard property window opens. Almost every field is read-only because the data is produced by importing or baking. The exception is **Antialiased**.

### Gaussian Splat > Splat Data

**Debug**

| Field | Meaning |
|---|---|
| **Source Filename** | The file the asset was imported from, or the capture it was trained from. |
| **Num Points** | Number of splats. |
| **SH Degree** | Highest spherical harmonic degree stored (0 to 3). Higher degrees hold more view-dependent colour and take more space. |
| **Bounds Min** / **Bounds Max** | The box that contains the splats, in centimetres, in the asset's own space. |
| **Estimated Serialized Size MB** | Estimated size of the splat data on disk. |
| **Encoding** | **Expanded** (full-precision floats) or **Compressed** (quantised). |
| **Representation** | **3D Gaussian** or **BB Splat (Textured Billboard)**. |
| **Antialiased** | The splat was trained with an anti-aliasing filter (Mip-Splatting, or gsplat's "antialiased" mode). Harmony then applies the matching opacity correction when drawing. Set automatically for splats trained in Harmony. The only editable field in this section. |
| **Source Asset Path** | For a decimated asset, the asset it was made from. Empty otherwise. |
| **Decimation Target Ratio** | For a decimated asset, the fraction that was kept. 1.0 means not decimated. |

Splats imported from `.ply` and the other splat formats never have **Antialiased** set (a `.harmony` file keeps the value it was exported with). If an imported splat was trained with an anti-aliasing filter, tick **Antialiased** on its asset and save it. Leave it off for ordinary splats: on them the correction dims and removes thin, distant splats. The console variable `r.Harmony.Tuning.Preprocess.OpacityCompensation 1` applies the same correction to every splat in the project regardless of the asset setting.

**Packed**

| Field | Meaning |
|---|---|
| **Unit Scale** | Scale from the source file's units to Unreal units. Always 100 (metres to centimetres). Assets with any other value are not drawn. |

**Compressed** (only meaningful when **Encoding** is Compressed)

| Field | Meaning |
|---|---|
| **Compressed Geometry Count** / **Compressed Feature Count** | Number of stored shape and colour entries. Equal to **Num Points** except for sparse `.npz` imports, which share entries between splats. |
| **Compressed Quantization** | The scale and zero point used to decode colour, opacity and size. |
| **Compression Format** | Format version and layout flags. |

### Lighting

Filled in by the Harmony Capture Rig. Empty for splats imported from files. The components that use this data are described in [Lighting and shadows](10_LightingAndShadows.md); how it is captured is in [Lighting capture](../Baking/06_LightingCapture.md).

**Lighting Probe**: a 360 degree HDR image of the scene taken from the Harmony Capture Rig's centre, plus what was measured with it.

| Field | Meaning |
|---|---|
| **Cubemap** | The HDR probe image. Empty means the asset has no probe. |
| **Radiance Scale** | Multiplier that turns the stored image back into scene brightness. |
| **Width** | Width of the probe's source image in pixels. |
| **Has Capture EV100** / **Capture EV100** | The exposure the capture, and so the splat, was rendered at. Lit meshes match the splat's brightness at this exposure. |
| **Capture Location** | Where the probe was taken, in the captured level. |
| **Has Sun** | Whether the captured level had a sun (its brightest directional light). |
| **Sun Direction** / **Sun Color** / **Sun Illuminance** | The sun as it arrives at the ground. |
| **Sun Fraction** | The sun's share of the light on the ground. A sun shadow removes this much light. |
| **Sun Reaches Stand** | Whether the sun reaches the spot where objects are placed. If not, placed objects get no sun shadow. |

**Shadow Proxy**: a small height map of the ground around the rig, so that caught shadows follow the ground's shape. Fields: **Size**, **Center**, **Half Extent**, **Min Height**, **Height Range**.

**Reflection Proxy**: the floor's shininess around the rig, and the plane reflections mirror about. Empty when the floor was too rough to show a reflection or the capture did not ask for one. Fields: **Size**, **Center**, **Half Extent**, **Plane Height**, **Plane Normal**, **Shiny Share**, **Floor Albedo**, **Floor Radiance**.

Variants share the shadow and reflection proxies. Each variant option can have its own lighting probe.

### Fallback

**Fallback Tiers** maps a tier name (**Low**, **Mid**, **High**) to a simplified, textured static mesh built from the same capture, with optional per-variant entries. This is the only editable section. It is filled in by the fallback mesh builder; see [Fallback mesh](../Baking/07_FallbackMesh.md).

Nothing loads or displays these meshes at runtime in this release. They are references for you to use.

### Gaussian Splat > Variants

**Variant Sets** lists the looks baked on top of the base splats. See [Variants](09_Variants.md) for using them and [Baking variants](../Baking/05_Variants.md) for making them.

| Field | Meaning |
|---|---|
| **Name** | The set's name, for example "Time of Day". |
| **Base Option** | The name of the option that the base splats show. |
| **Options** | The other options in the set. |
| Options > **Name** | The option's name, for example "Dusk". |
| Options > **Num Changed Points** | How many splats differ from the base in this option. |
| Options > **Bake Info** | A note recorded when the option was baked. |
| Options > **Lighting Probe** | The option's own probe, if its lighting differs from the base. |
| Options > **Stored Size MB** | Disk size of the option's data. |

Variant options are tied to the exact set of splats they were baked for. If the splats are replaced (re-import, decimation, retraining), the options are dropped.

---

## Property names for tools

These are the C++ property names, as they appear in a [.harmony file's](08_HarmonyFile.md) manifest and to Python. All splat data lives inside the `SplatData` struct.

### Summary fields (`SplatData`)

| Property | Type | Description |
|---|---|---|
| `SourceFilename` | `FString` | Source file |
| `NumPoints` | `int32` | Number of splats |
| `SHDegree` | `int32` | Highest SH degree stored |
| `BoundsMin` / `BoundsMax` | `FVector3f` | Bounds |
| `EstimatedSerializedSizeMB` | `float` | Estimated stored size |
| `Encoding` | `EHarmonyAssetEncoding` | `Expanded` or `Compressed` |
| `Representation` | `EHarmonyPrimitiveRepresentation` | `Gaussian3D` or `Billboard2D` |
| `bAntialiased` | `bool` | Trained with an anti-aliasing filter |
| `SourceAssetPath` | `FString` | Decimation source |
| `DecimationTargetRatio` | `float` | Decimation keep ratio |
| `UnitScale` | `float` | Always 100 |

### Expanded data

Present when `Encoding` is `Expanded`. Positions are in centimetres in the component's local space.

| Property | Type | Description |
|---|---|---|
| `PosRadius` | `TArray<FVector4f>` | XYZ position and radius, one per splat |
| `Color` | `TArray<FVector4f>` | RGBA, alpha in `.W`, one per splat |
| `Covariance` | `TArray<FVector4f>` | 3D covariance, two entries per splat |
| `SHCoeffs` | `TArray<FVector4f>` | Spherical harmonic coefficients: 1, 4, 9 or 16 per splat for degree 0, 1, 2 or 3 |

### BB Splat data

Present when `Representation` is `Billboard2D`. BB Splat assets are always Expanded and also use `PosRadius` and `SHCoeffs`.

| Property | Type | Description |
|---|---|---|
| `BillboardAxisX` / `BillboardAxisY` | `TArray<FVector4f>` | The two scaled, oriented axes of each billboard's plane |
| `BillboardTextureRGBA` | `TArray<uint8>` | One 16 x 16 RGBA8 texture per billboard, row by row |

### Compressed data

Present when `Encoding` is `Compressed`. Not every asset uses every array.

| Property | Type | Description |
|---|---|---|
| `CompressedGeometryCount` / `CompressedFeatureCount` | `int32` | Stored shape / colour entries |
| `CompressedQuantization` | struct | `ColorDC`, `ColorRest`, `Opacity`, `ScalingFactor`, each with `ZeroPoint` and `Scale` |
| `CompressionFormat` | struct | `Version`, `Preset`, `bUsesMortonOrdering`, `bUsesScaleCodebook`, `bUsesColorDCCodebook`, `bUsesSHPalette` |
| `CompressedPositionQuantized` | `TArray<uint16>` | Position offsets inside buckets |
| `CompressedPositionBucketCenter` / `CompressedPositionBucketExtent` | `TArray<FVector3f>` | Per-bucket centre and half-extent |
| `CompressedPosition` | `TArray<FVector3f>` | Older float position storage |
| `CompressedMetadataCompact` | `TArray<uint16>` | Packed opacity and size word per splat |
| `CompressedMetadataCodebookLabels` / `CompressedMetadataCodebook` | `TArray<uint8>` / `TArray<uint16>` | Optional shared table for that word |
| `CompressedMetadata` | `TArray<uint32>` | Older full-width form of that word |
| `CompressedCovariance` | `TArray<uint32>` | Packed shape |
| `CompressedSHDataBytes` | `TArray<uint8>` | Byte-packed SH coefficients |
| `CompressedSHData` | `TArray<uint32>` | Word-packed SH coefficients |
| `CompressedColorDCCodebookLabels` / `CompressedColorDCCodebook` | `TArray<uint8>` / `TArray<uint32>` | Older colour table form; not written by the current compressor |

### Other top-level properties

| Property | Type | Description |
|---|---|---|
| `LightingProbe` | `FHarmonyLightingProbe` | The base look's lighting probe |
| `ShadowProxy` | `FHarmonyShadowProxy` | Ground height map |
| `ReflectionProxy` | `FHarmonyReflectionProxy` | Floor material and mirror plane |
| `FallbackTiers` | `TMap<FName, FHarmonyFallbackTierAssets>` | Fallback meshes by tier name |
| `VariantSets` | `TArray<FHarmonySplatVariantSet>` | Baked variant sets |

---

## Good to know

- **The asset is a soft reference on the component.** A Harmony component loads its asset when it first needs it. Very large assets cause a hitch at that moment.
- **Do not edit the data by hand.** The read-only fields describe binary data stored alongside them. Changing counts or flags from a script without changing the data will break the asset.
- **The layout may change.** The plugin is Alpha. Use `.harmony` files, not `.uasset` files, to move assets between engine versions or projects.

---

## Next steps

- [Compression and decimation](05_CompressionAndDecimation.md)
- [.harmony files](08_HarmonyFile.md)
- [Variants](09_Variants.md)
