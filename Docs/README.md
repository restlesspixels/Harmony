# Harmony documentation

Harmony has two parts: a Gaussian splat renderer for Unreal Engine, and a scene baker that turns an Unreal level into a splat. If you are new, start with the guides. The Rendering and Baking sections are the reference.

For requirements, installation and release information, see the [main README](../README.md).

## Guides

Step-by-step walkthroughs.

| Guide | What it covers |
|---|---|
| [Render your first splat](Guides/01_RenderYourFirstSplat.md) | Install the plugin, import a `.ply`, place it in a level, check the look and play |
| [Bake an Unreal scene](Guides/02_BakeAnUnrealScene.md) | Capture and train a showroom-style interior, then view the result and add lit meshes |
| [Running on low-end hardware](Guides/03_LowEndHardware.md) | Composition modes, splat resolution, decimation, culling and the fallback mesh |

## Rendering

Importing splats and drawing them in the editor and at runtime. Works on Windows and macOS.

| Page | What it covers |
|---|---|
| [01 Installation](Rendering/01_Installation.md) | Requirements, installing from a zip or git, first launch |
| [02 Importing](Rendering/02_Importing.md) | Supported file formats, their constraints and the import workflow |
| [03 Placing splats](Rendering/03_PlacingSplats.md) | The Harmony Actor, the Harmony component on any actor, and the Harmony Volume |
| [04 Rendering settings](Rendering/04_RenderingSettings.md) | Project settings: composition, tonemapping, performance, culling, draw and masking |
| [05 Compression and decimation](Rendering/05_CompressionAndDecimation.md) | Compressed and Expanded assets, and reducing splat counts |
| [06 Asset reference](Rendering/06_AssetReference.md) | Everything stored in a Harmony Data Asset |
| [07 BB splats](Rendering/07_BBSplat.md) | Textured billboard splats: import layout, rendering and limitations |
| [08 The .harmony file](Rendering/08_HarmonyFile.md) | The engine-version-independent asset file, with a Python reader |
| [09 Variants](Rendering/09_Variants.md) | Switching variants at runtime: dropdowns, Blueprint, transitions and Variant Manager sync |
| [10 Lighting and shadows](Rendering/10_LightingAndShadows.md) | The Harmony Lighting component, the Shadow Catcher and reflections |
| [11 Debugging](Rendering/11_Debugging.md) | Buffer visualisations, `stat Harmony` and useful console variables |

## Baking

Capturing an Unreal scene and training it into a splat. These are editor tools that need Windows and an SM6-capable GPU.

| Page | What it covers |
|---|---|
| [01 Overview](Baking/01_Overview.md) | What baking is, what it needs, and what it costs in time, memory and disk |
| [02 Capture rig](Baking/02_CaptureRig.md) | Placing the rig, the view area, camera patterns, shot preview, custom views, exclusions and exposure |
| [03 Capture and train](Baking/03_CaptureAndTrain.md) | The capture buttons, progress, outputs, training options and retraining |
| [04 Dataset cache](Baking/04_DatasetCache.md) | The cache layout, the COLMAP format, external trainers and pruning |
| [05 Variants](Baking/05_Variants.md) | Authoring and baking variants (experimental) |
| [06 Lighting capture](Baking/06_LightingCapture.md) | The lighting probe, shadow proxy, floor reflection and how exposure carries through |
| [07 Fallback mesh](Baking/07_FallbackMesh.md) | Building simplified, textured meshes from a capture |
| [08 Automation](Baking/08_Automation.md) | Python and Blueprint entry points, and unattended runs |

## Where to look for common tasks

| I want to… | Read |
|---|---|
| Get a splat on screen quickly | [Render your first splat](Guides/01_RenderYourFirstSplat.md) |
| Fix washed-out or over-contrasty splat colours | [Rendering settings](Rendering/04_RenderingSettings.md) (Tonemapping) |
| Make a splat cheaper to draw | [Running on low-end hardware](Guides/03_LowEndHardware.md) |
| Cut a hole in a splat or hide part of it | [Placing splats](Rendering/03_PlacingSplats.md) (Harmony Volume) |
| Put a lit mesh in a splat with shadows | [Lighting and shadows](Rendering/10_LightingAndShadows.md) |
| Turn my Unreal level into a splat | [Bake an Unreal scene](Guides/02_BakeAnUnrealScene.md) |
| Train a capture in another tool | [Dataset cache](Baking/04_DatasetCache.md) |
| Move a splat asset between engine versions | [The .harmony file](Rendering/08_HarmonyFile.md) |
