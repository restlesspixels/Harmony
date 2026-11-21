# Installation

Harmony ships as a precompiled plugin. You copy it into your project's `Plugins` folder and open the project; there is nothing to compile. This page covers what you need, the two ways to get the plugin, and how to check it is working.

> Harmony is **Alpha**. Asset formats, settings and Blueprint APIs may change between releases.

---

## Requirements

| | |
|---|---|
| **Unreal Engine** | **5.9.0 exactly**. The binaries are built against that engine build and will not load in another version. |
| **Platforms (rendering)** | Windows (Direct3D 12) and macOS (Apple Silicon and Intel, Metal). Works in the editor and in packaged games. |
| **GPU (rendering)** | A GPU that runs Unreal's desktop renderer at Shader Model 5 or above. |
| **Project type** | Blueprint-only and C++ projects both work. C++ modules can depend on the `Harmony` module. |

Scene baking (capturing an Unreal scene and training it into a splat) and the fallback-mesh builder have stricter requirements: Windows with an SM6 GPU. See the [Baking overview](../Baking/01_Overview.md). Everything on the Rendering pages works without them.

Each Harmony release targets one engine version. A newly supported engine version gets its own branch and its own release zip.

---

## Installing the plugin

Close the Unreal Editor first.

### Option 1: release zip

1. Download the zip for your engine version from the [Releases page](https://github.com/restlesspixels/Harmony/releases), for example `Harmony-0.0.1-UE5.9.zip`.
2. Find your project's root folder (the one that contains the `.uproject` file).
3. Create a `Plugins` folder there if it does not exist.
4. Extract the zip into `Plugins/`. The zip already contains a single top-level `Harmony/` folder, so the plugin ends up in `Plugins/Harmony/`.

### Option 2: git

From your project's root folder:

```bash
git clone -b ue5.9 https://github.com/restlesspixels/Harmony.git Plugins/Harmony
```

Use the branch that matches your engine version (`ue5.9`). The `main` branch follows the newest supported engine.

### Check the folder layout

Either way, `Harmony.uplugin` must sit directly inside `Plugins/Harmony/`:

```
MyProject/
  MyProject.uproject
  Plugins/
    Harmony/
      Harmony.uplugin
      Binaries/
      Docs/
      Intermediate/
      Resources/
      Source/
      README.md
      THIRD_PARTY_NOTICES.txt
```

A release zip contains exactly these. `Source/` holds only the public headers and module build files, so C++ modules can depend on Harmony; there is no implementation source to compile.

A common mistake is an extra nested folder (`Plugins/Harmony/Harmony/Harmony.uplugin`), which happens if you extract the zip into `Plugins/Harmony/` rather than `Plugins/`. The editor will not find the plugin's binaries in that case.

---

## First launch

1. Open your project. The editor starts normally; you should **not** be asked to rebuild any modules. If you are, see [Troubleshooting](#troubleshooting).
2. Harmony is enabled by default. To confirm, open **Edit > Plugins**, search for **Harmony** (category **Rendering**) and check that it is ticked.

Harmony also switches on two engine plugins it depends on: **Movie Render Queue** and **Variant Manager Content**. You do not need to enable them yourself.

On macOS the binaries are Developer ID signed and notarised, so Gatekeeper accepts them after download.

---

## Checking that it works

- **Edit > Project Settings > Plugins** lists **Harmony** and **Harmony Import**.
- The **Tools** menu has a **Harmony** section.
- Searching **Place Actors** for "Harmony" finds **Harmony Actor**.
- Dragging a `.ply` splat file into the Content Browser imports it as a Harmony Data Asset.

Then continue with [Importing splats](02_Importing.md), or follow the step-by-step guide [Render your first splat](../Guides/01_RenderYourFirstSplat.md).

---

## Updating

1. Close the editor.
2. Zip install: delete `Plugins/Harmony/` and extract the new zip in its place. Git install: run `git pull` inside `Plugins/Harmony/`.
3. Open the project again.

Your Harmony settings live in your project's `Config` folder, not in the plugin folder, so they survive an update. Because the plugin is Alpha, read the release notes before updating a project you care about, and keep the original source files of your splats.

---

## Troubleshooting

| Symptom | Likely cause |
|---|---|
| "Missing modules" or an offer to rebuild them on launch | The engine version does not match. Harmony 0.0.1 needs Unreal Engine 5.9.0 exactly. Rebuilding cannot work because the plugin ships without its implementation source. |
| Harmony is not listed in **Edit > Plugins** | The folder is nested one level too deep, or `Harmony.uplugin` is missing from `Plugins/Harmony/`. |
| Splat files are ignored when dragged into the Content Browser | The plugin is not loaded. Check the two rows above. |

---

## Good to know

- There is no installer. Installation is per project.
- The plugin is closed source. A release contains binaries plus the public headers needed to build against it.
- Rendering is tested on Windows with Direct3D 12 and on an Apple M1 Mac with Metal. Other platforms are not supported in this release.

---

## Next steps

- [Importing splats](02_Importing.md): bring splat files into your project
- [Placing splats](03_PlacingSplats.md): put them in a level
- [Rendering settings](04_RenderingSettings.md): how splats combine with Unreal content
