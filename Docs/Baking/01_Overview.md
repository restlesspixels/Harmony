# Scene Baking Overview

Scene baking turns an Unreal scene into a Gaussian splat without leaving the editor. You place a Harmony Capture Rig, press one button, and Harmony renders a few thousand views of the level, trains a splat from them on your GPU and saves it as a Harmony Data Asset.

This page explains what you get, what it costs and what it cannot do. The rest of this section covers each step in detail.

## Why bake a scene

A level built with Lumen, Nanite, high-resolution textures and many lights is expensive to draw. A baked splat stores what that level *looks like* from the area you captured, with the lighting, reflections and materials already in it. Drawing it costs the same whatever the original scene cost.

- **One asset instead of a heavy level.** The result is a single Harmony Data Asset (the motor court example below is 59 MB). It renders anywhere the Harmony renderer runs, with the look you captured. See [Installation](../Rendering/01_Installation.md) for supported platforms.
- **Regular Unreal content still works inside it.** The splat is drawn alongside meshes, so you can put a vehicle, a character or a product into the baked environment.
- **Lit objects match the environment.** The capture also records a 360 degree lighting probe, the ground shape and (for shiny floors) the floor material. A Harmony Actor uses them to light your meshes with the scene's own lighting, catch their shadows on the splat and reflect them in the floor. See [Lighting Capture](06_LightingCapture.md).
- **Variants (experimental).** Bake several looks of the same scene, such as a time of day or a material option, into one asset and switch between them at runtime. See [Variants](05_Variants.md).
- **No external tools.** Rendering uses Movie Render Queue and training runs as GPU compute inside Unreal. There is no CUDA, Python environment or separate trainer to install.

A baked splat is a view of the scene from the captured area. It is not a replacement for a level you need to walk through freely, relight or edit. See [Limitations](#limitations).

## The pipeline at a glance

```
Capture Rig  ->  render views        ->  dataset          ->  train in editor  ->  Harmony Data Asset
(camera          (Movie Render Queue,    (images, poses,      (live preview)       (+ lighting probe,
 positions)       in a PIE window)        depth, masks)                              shadow and floor data)
```

1. **Place a Harmony Capture Rig.** Its green include box marks where the viewer will stand. The rig fills the box with camera positions. See [The Capture Rig](02_CaptureRig.md).
2. **Press Capture and Train.** Harmony builds a Movie Render Queue job in memory and renders every shot in a Play In Editor window. You do not set up a Level Sequence or a render preset.
3. **Dataset.** The rendered views become a training dataset: images, camera poses, depth, masks and starting points. It is kept on disk as a [dataset cache](04_DatasetCache.md), so you can train again later without rendering.
4. **Training.** The splat trains in the editor while you watch it sharpen in the viewport. The editor stays usable.
5. **Asset.** The result is saved as a Harmony Data Asset. Place a Harmony Actor at the world origin, assign the asset and you are looking at the baked scene. See [Capture and Train](03_CaptureAndTrain.md).

From the same capture you can also [retrain](03_CaptureAndTrain.md#retrain-from-last-capture) with other settings, [bake variants](05_Variants.md), build a [fallback mesh](07_FallbackMesh.md), or hand the dataset to an [external trainer](04_DatasetCache.md#using-a-cache-with-an-external-trainer).

## Requirements

Scene baking is an editor tool. A splat it produces renders on every platform the Harmony renderer supports, but the capture, the training and the fallback mesh builder have their own requirements:

| Requirement | Details |
|---|---|
| Operating system | Windows. Baking is not supported on macOS. |
| GPU | A Shader Model 6 GPU (D3D12 SM6 or Vulkan SM6). Only an RTX 3090-class card on D3D12 has been tested. |
| Plugins | Movie Render Queue and Variant Manager Content. Harmony enables both automatically. |
| The level | Saved at least once. Play In Editor must not be running when you start. |

Training has only been tested on an RTX 3090 with 24 GB of video memory. There is no measured minimum, so treat cards with less memory as untested.

## What it costs

These figures are examples from an RTX 3090-class GPU at the default capture resolution of 1024 (training images are 512). Your scenes will differ.

### Time

| Scene | Views | Render | Dataset and starting points | Training | End to end |
|---|---|---|---|---|---|
| Small test | 234 | | | | about 3.3 min |
| Motor court (hard surface, exterior) | 4,212 | 16.8 min | 8.0 min | 3.3 min (30,000 steps) | 29 min |
| Foliage-heavy exterior | 4,200 to 4,800 | 21 to 34 min | 17 to 45 min | 14 to 17.5 min (100,000 steps) | 60 to 78 min |

The motor court run reached a validation PSNR of 33.0 dB. A showroom interior trained for 30,000 steps took about 190 seconds of training, produced about 620,000 splats and reached 34.2 dB.

Rendering is usually the largest part. The rig shows an estimate before you start (see [Rows Summary](02_CaptureRig.md#rows-jittered-recommended)). Each variant option adds another render of every view plus a short training run.

### Memory

The dataset is held in RAM while training: about 6.8 GB for 4,500 views at 512 pixels. The fallback mesh builder peaks at about 15 GB of RAM.

### Disk

Each capture is cached on disk, about 3.6 GB for 4,000 views. By default Harmony keeps the three newest caches and deletes older ones. See [Dataset Cache](04_DatasetCache.md).

## The pieces

| Piece | Where you find it | Page |
|---|---|---|
| Harmony Capture Rig | **Place Actors** panel, search for "Harmony Capture Rig" | [The Capture Rig](02_CaptureRig.md) |
| Capture buttons | Rig **Details** panel, **Capture** category | [Capture and Train](03_CaptureAndTrain.md) |
| Project defaults | **Edit > Project Settings > Plugins > Harmony Training** | [Capture and Train](03_CaptureAndTrain.md#project-settings) |
| Dataset cache | `Saved/HarmonyTraining/Datasets` in your project | [Dataset Cache](04_DatasetCache.md) |
| Variants | Rig **Details** panel, **Variants** category | [Variants](05_Variants.md) |
| Lighting probe, shadow and floor data | Captured with the views, stored in the asset | [Lighting Capture](06_LightingCapture.md) |
| Fallback mesh builder | Rig **Details** panel, **Capture** category | [Fallback Mesh](07_FallbackMesh.md) |
| Python and Blueprint entry points | `unreal.HarmonyCaptureJobSubsystem` and others | [Automation](08_Automation.md) |

For a start-to-finish walkthrough, see the guide [Bake an Unreal Scene](../Guides/02_BakeAnUnrealScene.md).

## Limitations

Harmony is in Alpha, and baking is its newest part. Know these before you plan around it.

**What a baked splat is**

- It looks right from inside the captured area and gets worse the further you move outside it. Size the include box to where viewers will actually be.
- The lighting is baked in. You cannot relight the splat, and moving objects in the original level are frozen as they were captured.
- Training images are half the capture resolution. Very fine detail such as gravel or paving grain is softened.
- A splat asset holds at most 2,097,120 splats. The default training cap is 1,000,000.

**During capture**

- Nothing pauses the scene. Wind, world position offset animation, Niagara effects and animated actors keep moving between shots and will blur or ghost. Stop or freeze them first.
- Every view must share one exposure. Harmony locks it for you; see [Exposure](02_CaptureRig.md#exposure).
- The capture warms up for a fixed 32 frames. A bright Lumen interior can take longer to settle, and the first shots may come out slightly dark.
- Keep one capture rig per level. Exclusions, the capture radius and the lighting probe are read from the first rig found.
- Do not run another Movie Render Queue render at the same time. Two renders at once have crashed the GPU driver.
- Cancelling cannot interrupt an asset that is already being saved.

**Sky and glass**

- By default the sky and distant scenery are trained as splats. You can limit training to a radius and let Unreal draw the sky instead, but that also leaves glass seen against the sky untrained. See [Capture radius](02_CaptureRig.md#capture-radius).
- Depth is not recorded behind glass or on glossy surfaces, by design. This affects the starting points and the fallback mesh, not the look of the trained splat.

**Workflow**

- Training an asset again removes its baked variants and replaces its lighting probe. Bake variants last.
- Variants are experimental.
- The fallback mesh is not switched in automatically at runtime. See [Fallback Mesh](07_FallbackMesh.md).
- Refining an existing splat with extra views is available from Python only.
