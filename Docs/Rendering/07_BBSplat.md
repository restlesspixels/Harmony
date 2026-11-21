# BB Splat

BB Splat (billboard splatting) is a variation on Gaussian splatting in which each primitive is a small, flat, textured rectangle instead of a soft 3D blob. Harmony can import BB Splat checkpoints and draw them alongside ordinary Gaussian splats, using the same renderer.

Use this page if you train with BB Splat. If you only work with normal Gaussian splats, you can skip it.

---

## Importing

A BB Splat checkpoint is a PLY file with two companion archives. All three must be in the same folder:

| File | Contents |
|---|---|
| The checkpoint `.ply` | Positions, two-axis scales, rotations and colour (spherical harmonics). Binary little-endian. |
| `texture_alpha.npz` | Contains `texture_alpha.npy`, shape `[N, 16, 16]`, 8-bit. |
| `texture_color.npz` | Contains `texture_color.npy`, shape `[N, 3, 16, 16]`, 8-bit. |

To import:

1. Drag the `.ply` into the Content Browser, or use **Import**.
2. Harmony notices the two `.npz` files beside it and imports the PLY as a BB Splat instead of a Gaussian splat.

If either companion file is missing, the PLY is not imported as a BB Splat and the import fails. Check that both archives are beside the PLY and have exactly these names.

The archives must use BB Splat's standard layout: C-order, unsigned 8-bit arrays. During import Harmony combines the alpha and colour arrays into one 16 x 16 RGBA texture per billboard and stores it in the asset.

The resulting asset shows **Representation: BB Splat (Textured Billboard)** in its tooltip and Details. Place it in a level exactly like any other Harmony Data Asset; see [Placing splats](03_PlacingSplats.md).

---

## How it renders

BB Splat and Gaussian primitives go through the same pipeline:

1. Each primitive is transformed and culled.
2. All visible primitives from all Harmony components, of both kinds, are put in one list.
3. That list is sorted back to front once.
4. Each primitive is drawn as a quad. Gaussians evaluate their soft falloff; billboards sample their texture.

So a level can mix Gaussian assets and BB Splat assets freely, and their primitives blend with each other in the correct order. The rest of the pipeline is shared too: composition with Unreal geometry, tonemapping modes, near fade, view-dependent colour and temporal upscaling support. See [Rendering settings](04_RenderingSettings.md).

---

## Sorting limitation

Sorting uses one depth value per primitive: the depth of its centre. A Gaussian is roughly round, so that is a good approximation. A billboard is a tilted plane, so parts of it can be nearer or further than its centre.

Most of the time this does not show. It can become visible where a large, steeply tilted billboard passes through other semi-transparent splats over a noticeable area of the screen. The symptom is a local seam, or a pop in blending order as the camera moves.

There is no setting to fix this. It is less likely the smaller the billboards are.

---

## Limitations

- **Always Expanded.** BB Splat assets cannot be compressed. The compressed format has no way to store a billboard's orientation and texture. **Import Compressed** is ignored for them and **Convert to Compressed** is not offered.
- **No decimation.** **Decimate...** and import-time decimation do not apply.
- **Fixed texture size.** Textures are 16 x 16 RGBA8 per billboard. Other sizes are not accepted.
- **Binary little-endian PLY only.**
- **Larger assets.** Each billboard carries a 1 KB texture on top of its other data, and the asset is uncompressed, so BB Splat assets take more disk space and video memory per primitive than compressed Gaussian assets.
- **Centre-depth sorting**, as described above.
- **Less tested.** BB Splat support has had less performance and quality testing than Gaussian rendering. Profile on your target hardware before relying on it.
- **No animated variant transitions.** Those need a Compressed asset. See [Variants](09_Variants.md).

---

## Next steps

- [Importing splats](02_Importing.md)
- [Placing splats](03_PlacingSplats.md)
- [Asset reference](06_AssetReference.md#bb-splat-data): the stored data
