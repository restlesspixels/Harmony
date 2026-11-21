# Dataset Cache

Every capture is saved to disk as a dataset cache: the rendered views, their camera poses and everything else training needs. The cache is what lets you retrain without rendering, bake variants, build a fallback mesh, or train the same views in another tool.

## Where it lives

Caches are written under the **Dataset Cache Root**, which defaults to `Saved/HarmonyTraining/Datasets` in your project folder. Change it in **Edit > Project Settings > Plugins > Harmony Training** if your project drive is short of space.

Each capture gets its own folder:

| Capture | Folder name |
|---|---|
| A base capture | `<Level>_Base_<yyyymmdd_hhmmss>` |
| A variant option | `<Level>_<Set>_<Option>_<yyyymmdd_hhmmss>` |

A cache of about 4,000 views at the default resolution is around 3.6 GB.

## What is in a cache

| Path | Contents |
|---|---|
| `harmony_dataset.json` | The manifest: level, job name, output asset, image count and size, and which channels exist. Its presence marks the folder as a cache. |
| `sparse/0/cameras.txt` | Camera intrinsics, COLMAP text format, `PINHOLE` model. |
| `sparse/0/images.txt` | One camera pose per view, COLMAP text format. |
| `sparse/0/points3D.txt` | The starting points for training (up to 1,000,000), sampled from the scene's depth. |
| `images_2/frame_000000.png` ... | The training images: 8-bit sRGB, tonemapped, exposure-locked, at half the capture resolution. |
| `masks_2/` | Which pixels are trained (excluded actors and anything beyond the capture radius are masked out). |
| `depth_2/` | Depth per view as raw 16-bit float files (`.f16`), in metres. 0 means no depth: sky, masked pixels, and surfaces behind glass or on glossy materials. |
| `depth_opaque_2/` | Depth of opaque geometry only, so it sees through glass. |
| `backgrounds_2/` | A sky-only render of each view. |
| `glossy_2/`, `roughness_2/`, `reflectance_2/`, `normal_2/` | Material buffers per view. Normals are world-space in Unreal's axes. |
| `mrq_capture.json` | A summary of the render. |
| `lighting_probe.hdr`, `lighting_probe.json` | The [lighting probe](06_LightingCapture.md) and its measurements. |
| `shadow_height.r16`, `shadow_sun.hdr`, `shadow_total.hdr` | The shadow proxy: ground heights and the sun's share of the light. |
| `reflection_material.png` | The floor material bake, when one was made. |

The `_2` suffix is the downscale factor. The rig always uses 2.

Variant option caches hold a few extra files that record what the option changed.

## Pruning

**Max Cached Datasets** (default 3) limits how many caches are kept. After each capture, the oldest folders beyond that count are deleted. Set it to 0 to keep everything.

The count covers every cache under the root, across all levels, and each variant option is a cache of its own. Caches in use by a running bake are protected until it finishes, but the next capture can remove them. This matters because several features need a cache to still exist:

| Feature | Needs |
|---|---|
| **Retrain From Last Capture** | The base capture of this level and asset. |
| **Bake Variants** with **Reuse Base for Variants** | The base capture. |
| **Build Fallback Mesh From Last Capture** | The base capture, plus each option's capture for variant fallbacks. |

If you bake variants, build fallback meshes, or work on more than one level, raise **Max Cached Datasets** or set it to 0, and clear old caches by hand. To delete a cache, delete its folder.

Two more folders under `Saved/HarmonyTraining` are not pruned and are safe to clear when no job is running: `Render`, the Movie Render Queue output folder, and `MRQ`, which collects training start-up data. The start-up data is only used by the training that follows a capture in the same job. **Retrain From Last Capture** and the other features above read the dataset cache, not these folders.

## Retraining from a cache

- On the rig: **Retrain From Last Capture** uses the newest base capture of the level whose recorded output asset matches the rig's **Output Asset**. See [Capture and Train](03_CaptureAndTrain.md#retrain-from-last-capture).
- From Python you can train any cache by name or path, into any asset. See [Automation](08_Automation.md).

Loading a cache takes about a minute before training starts.

## Using a cache with an external trainer

A cache is a standard COLMAP text dataset with pre-downscaled images, the layout most Gaussian splat trainers read. Caches from Harmony have been trained in Postshot and VkSplat. Point the trainer at the cache folder and check these three things.

**1. Intrinsics are full resolution, images are half.** `cameras.txt` describes the capture resolution, for example:

```
1 PINHOLE 1024 1024 512 512 512 512
```

while the images in `images_2` are 512 x 512. This is the usual convention for a trainer that takes a downscale factor: tell it to use factor 2 (or the `images_2` folder). If your trainer has no such option, halve the width, height, focal lengths and principal point in `cameras.txt`.

**2. Units and axes.** Positions are in metres, in COLMAP's camera convention. Relative to Unreal:

| COLMAP | Unreal |
|---|---|
| X | X |
| Y | -Z |
| Z | -Y |

So a COLMAP point `(x, y, z)` in metres is the Unreal point `100 * (x, -z, -y)` in centimetres.

**3. The images are display-referred.** They are tonemapped 8-bit sRGB at the locked exposure, so they look like the viewport did.

The extra channels (masks, depth, backgrounds, material buffers) are Harmony's own. Other trainers ignore them, which means excluded actors and capture-radius masking are not applied unless your trainer can read `masks_2`.

### Bringing the result back

Import the trained `.ply` like any other splat (see [Importing](../Rendering/02_Importing.md)). The PLY importer converts positions with the same `100 * (x, -z, -y)` mapping that the in-editor trainer uses, so a splat trained from a cache lines up with the level when its Harmony Actor sits at the origin with no rotation or scale, the same as one trained in the editor. This holds as long as your trainer keeps the cache's coordinate frame; a trainer that re-centres or rescales the scene will not line up.

An externally trained splat carries no lighting probe, shadow data or variants. Those are written by Harmony's own training job. You can add the cache's lighting probe, shadow and floor reflection data to the imported asset afterwards with `set_lighting_probe_from_cache` in Python; see [Automation](08_Automation.md).

## Good to know

- The rig's buttons always write a cache, whatever **Cache Capture Datasets** says. That setting applies to manual Movie Render Queue jobs.
- Harmony's own loader reads the `PINHOLE` and `SIMPLE_PINHOLE` camera models only.
- A capture made with **Shot Set** on **Custom Views Only** is stored as a base capture. It becomes the "last capture" for **Retrain From Last Capture** until you capture the full set again.
- The manifest records the level and output asset a cache belongs to. The rig's buttons use that to find "the last capture"; from Python you can train any cache explicitly by path.
