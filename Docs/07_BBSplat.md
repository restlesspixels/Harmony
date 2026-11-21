# BB Splat

Harmony can render BB Splat's textured, oriented 2D primitives alongside its existing 3D Gaussian primitives. This support uses Harmony's existing hardware-raster pipeline; it does not adopt BB Splat's CUDA tile renderer.

## Import

Select the checkpoint PLY in the Content Browser. The PLY directory must also contain:

- `texture_alpha.npz`
- `texture_color.npz`

The archives must use BB Splat's normal C-order uint8 layout: alpha `[N,16,16]` and color `[N,3,16,16]`. Harmony reconstructs BB Splat's alpha residuals during import and stores a 16 x 16 RGBA8 texture for each billboard.

BB Splat assets are marked `Billboard2D` and remain Expanded. Gaussian compression and decimation are intentionally disabled for them.

## Runtime path

Billboards and ordinary Gaussians use separate source buffers and preprocess shaders, then join the same runtime flow:

1. Each primitive is transformed and culled during preprocess.
2. Both representations write into the same stable preprocessed-splat address space.
3. Both append a center-depth key and global primitive index to the same visible list.
4. Harmony performs one global back-to-front sort.
5. The existing indirect quad draws branch by representation. Billboards fetch four clip-space corners and sample their texture; Gaussians use the existing projected covariance path.

This means separate Harmony actors can use Gaussian and BB Splat assets in the same scene, and their primitives interleave in one sorted draw. The normal early proxy, late hybrid, depth-rejection, fading, SH-color, and temporal-jitter structure remains in place.

Textures are uploaded into 1024 x 1024 RGBA8 texture-array slices. Each slice contains 4096 fixed 16 x 16 tiles, so a texture lookup needs only the billboard's local runtime index.

## Ordering limitation

The common sort uses one depth value per primitive: its adjusted center depth. The billboard itself is a tilted plane and receives perspective-correct texture coordinates and per-fragment plane depth, but blending order is still chosen from its center.

This is usually a consideration rather than a systemic failure. It can become visible where a large, strongly tilted billboard crosses another translucent splat over a meaningful screen area. The likely artifact is a localized blend-order seam or pop as the camera moves.

A practical future mitigation is selective subdivision: split only large or high-depth-gradient billboards into smaller textured quads before sorting. That improves the depth approximation while preserving the current pipeline and should cost far less than full per-pixel order-independent transparency. Exact per-pixel translucent ordering would require a substantially different renderer and would reduce the performance benefit this representation is intended to provide.

## Current constraints

- Companion textures are fixed at 16 x 16 RGBA8 per billboard.
- BB Splat assets are not supported by Gaussian compression or decimation.
- The importer currently accepts binary little-endian PLY checkpoints.
- Center-depth sorting has the tilted-plane limitation described above.
- Rendering has been integrated into the current non-tile pipeline; performance and visual quality still need profiling on target desktop and mobile hardware.
