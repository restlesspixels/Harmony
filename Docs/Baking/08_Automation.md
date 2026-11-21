# Automation

Everything the capture rig's buttons do can be driven from Python or Blueprint, so a bake can run overnight or as part of a build. This page lists the entry points and shows two short scripts.

Harmony has no console commands for baking. The functions below are exposed to both Python and Blueprint; the examples use Python.

## Before you script

- **Baking needs a real editor with a GPU.** The capture renders through Movie Render Queue and training runs on the GPU, on Windows with a Shader Model 6 GPU.
- **Capture jobs need a ticking editor.** A capture job runs over many editor frames. Start the editor normally and run your script with `-ExecCmds="py your_script.py"`. Do not use `-run=pythonscript`: a commandlet never ticks, so the job would start and never progress.
- **The functions return at once.** A start function returns an empty string when the job started, or the reason it could not. Poll for the result.

## Capture jobs

`unreal.HarmonyCaptureJobSubsystem` is the scripting side of the rig's buttons. Get it with `unreal.get_editor_subsystem(unreal.HarmonyCaptureJobSubsystem)`.

| Function | What it does |
|---|---|
| `capture_and_train(rig)` | The **Capture and Train** button. |
| `capture_only(rig)` | The **Capture Only** button. |
| `bake_variants(rig)` | The **Bake Variants** button. |
| `retrain_from_last_capture(rig)` | The **Retrain From Last Capture** button. |
| `cancel()` | Stops the running job. |
| `is_busy()` | True while a job is running. |
| `get_status()` | A short description of what the job is doing now. |
| `get_last_result()` | How the last job ended: `"OK: ..."` or `"ERROR: ..."`. Empty before the first job ends. |
| `find_last_base_capture(map_name, output_asset)` | Path of the newest base capture cache for a level and asset, or empty. |
| `find_last_option_capture(map_name, output_asset, variant_set, option)` | The same for one variant option. |

The four start functions return `""` on success. Otherwise they return the same message the editor shows as a notification, such as `save the level first ...`.

The rig is an ordinary actor, so its settings are editor properties. The **Capture** category is one struct property named `capture`:

```python
cap = rig.get_editor_property("capture")
cap.set_editor_property("output_asset", "/Game/Splats/SPL_Showroom")
cap.set_editor_property("training_steps", 30000)
rig.set_editor_property("capture", cap)
```

The rig also has `generate_camera_samples()` to rebuild its camera positions and `add_custom_view(location, rotation, label, cluster_size)` to append a [custom view](02_CaptureRig.md#custom-views).

### Example: capture and train unattended

This script opens a level, starts **Capture and Train** on its rig, logs the status as it changes and quits the editor when the job ends.

```python
# bake_showroom.py
import unreal

MAP = "/Game/Maps/Showroom"
ASSET = "/Game/Splats/SPL_Showroom"

state = {"ticks": 0, "job": None, "status": ""}


def start():
    if not unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).load_level(MAP):
        return "cannot open " + MAP
    actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors()
    rig = next((a for a in actors if isinstance(a, unreal.HarmonyCaptureRig)), None)
    if rig is None:
        return "no Harmony Capture Rig in " + MAP

    cap = rig.get_editor_property("capture")
    cap.set_editor_property("output_asset", ASSET)
    cap.set_editor_property("training_steps", 30000)
    rig.set_editor_property("capture", cap)

    state["job"] = unreal.get_editor_subsystem(unreal.HarmonyCaptureJobSubsystem)
    return state["job"].capture_and_train(rig)   # "" when started


def finish(result):
    unreal.log("Harmony bake finished: " + result)
    unreal.unregister_slate_post_tick_callback(state["handle"])
    unreal.SystemLibrary.quit_editor()


def tick(delta_seconds):
    state["ticks"] += 1
    if state["ticks"] < 120:          # let the editor finish starting up
        return
    if state["job"] is None:
        error = start()
        if error:
            finish("ERROR: " + error)
        return
    status = state["job"].get_status()
    if status != state["status"]:
        state["status"] = status
        unreal.log("Harmony bake: " + status)
    if not state["job"].is_busy():
        finish(state["job"].get_last_result())


state["handle"] = unreal.register_slate_post_tick_callback(tick)
```

Run it with:

```
UnrealEditor.exe "C:\Projects\MyProject\MyProject.uproject" -ExecCmds="py C:/Scripts/bake_showroom.py"
```

To bake variants instead, call `bake_variants(rig)`. The rig's variant source must already be set up in the level; see [Variants](05_Variants.md).

## Training without a capture

Two more classes train from data that is already on disk.

### `unreal.HarmonyTrainingEditorSubsystem`

Non-blocking training with the live preview. Like capture jobs, it needs a ticking editor. One training session runs at a time.

| Function | What it does |
|---|---|
| `start_training_from_cache(cache_dir, request)` | Trains a [dataset cache](04_DatasetCache.md), given as a folder or a cache name. Returns `""` or the error. |
| `start_training(request)` | Trains any COLMAP dataset on disk. Set the request's `colmap_sparse_dir` and `images_dir`. |
| `list_dataset_caches()` | Cache folders, newest first. |
| `get_status()` | A status struct: `state`, `step`, `steps`, `num_splats`, `loss`, `psnr`, `ms_per_step`, and at the end `val_psnr_mean`, `val_psnr_min` and `result`. |
| `pause_training()`, `resume_training()` | Pause and resume. |
| `cancel_training()` | Stops and discards the session. |
| `stop_and_finalize()` | Stops early and saves what has been trained so far, as if the step count had been reached. |
| `set_live_preview(enabled)` | Turns the viewport preview on or off. |
| `is_training()` | True while a session is loading, training, paused or saving. |
| `on_finished` | An event fired with a success flag and the result line. |

The request is an `unreal.HarmonyTrainingRequest`. Its main fields:

| Field | Default | What it does |
|---|---|---|
| `steps` | 0 | Training steps. 0 uses the project default. |
| `options_json` | Empty | [Training options](03_CaptureAndTrain.md#training-options), over the project defaults. |
| `asset_path` | Empty | Package path of the asset to write, such as `/Game/Splats/SPL_Test`. |
| `out_ply` | Empty | Optional path of a PLY to write as well. |
| `live_preview` | True | Draw the splat in the viewport while training. |
| `frame_budget_ms` | 33 | Editor frame time to keep to, training included. 0 trains as fast as possible and makes the editor unresponsive. |

### Example: train the newest cache into a test asset

Run this from the editor's Python console or **Tools > Execute Python Script** while the editor is open.

```python
import unreal

training = unreal.get_editor_subsystem(unreal.HarmonyTrainingEditorSubsystem)
caches = training.list_dataset_caches()          # newest first

request = unreal.HarmonyTrainingRequest()
request.set_editor_property("steps", 30000)
request.set_editor_property("options_json", '{"cap_max": 500000}')
request.set_editor_property("asset_path", "/Game/Splats/SPL_Showroom_Light")

error = training.start_training_from_cache(caches[0], request)
unreal.log(error or "training started")
```

Later, `training.get_status().get_editor_property("state")` reads `Done`, `Failed` or `Cancelled` when the session has ended.

Note that this trains the splats only. The lighting probe is imported by the capture job, not by this call. To add it, see `set_lighting_probe_from_cache` below.

### `unreal.HarmonyTrainingEditorLibrary`

Blocking helper functions. Each runs to completion before returning, so the editor is frozen meanwhile, and they do not need a ticking editor. Most return a line starting with `OK:` or `ERROR:`.

| Function | What it does |
|---|---|
| `train_from_cache(cache_dir, steps, out_ply, options_json, out_asset)` | Trains a cache, blocking. `steps` 0 and missing options use the project defaults. `out_ply` and `out_asset` are optional. |
| `list_dataset_caches()` | Cache folders, newest first. |
| `refine_from_caches(init, base_cache, refine_cache, steps, out_ply, options_json, out_asset, refine_repeat)` | Fine-tunes an existing splat on its base capture plus a second capture. See below. |
| `set_lighting_probe_from_cache(asset, set_name, option_name, dir)` | Imports a cache's lighting probe into an asset, or into one variant option. Pass `""` (or `"None"`) for both names to set the asset's own probe, which also imports the cache's shadow and floor reflection data. |
| `clear_lighting_probe(asset, set_name, option_name)` | Removes the asset's probe, or one option's. |
| `capture_lighting_probe(location_cm, long_lat_width, out_dir, ...)` | Captures a lighting probe of the open level at a location, without a full capture. |
| `export_asset_to_ply(asset_object_path, ply_path)` | Writes an asset's splats as a PLY. |
| `export_asset_to_ply_files(asset, out_dir)` | The **Export to PLY...** menu action: the base plus one PLY per variant option. |
| `write_asset_from_ply(ply_path, long_package_name, compress)` | Creates a Harmony Data Asset from a PLY in the trainer's format. |
| `describe_variant_options(asset)` | One line per baked variant option with its stored size. |

Because they do not need a ticking editor, these blocking functions can also run in a commandlet (`-run=pythonscript -script=...`), with one condition: add `-AllowCommandletRendering`. Training needs a real GPU device, and without that flag (or with `-nullrhi`) `train_from_cache` returns an error saying there is no usable RHI. The device must still be a Shader Model 6 GPU, as for any training.

The library also contains the individual steps of a variant bake and a number of test helpers. They are not needed for normal use and may change.

### Refining with custom views

`refine_from_caches` improves an existing splat for a few specific viewpoints. It is the one part of the [custom views](02_CaptureRig.md#custom-views) workflow that has no button yet.

1. Train the scene as usual. Keep its base cache.
2. Add custom views to the rig, set **Shot Set** to **Custom Views Only** and run **Capture Only**. Note the new cache folder.
3. Call `refine_from_caches` with the existing asset's object path as `init` (for example `/Game/Splats/SPL_Showroom.SPL_Showroom`), the base cache, the new cache, a step count, and the asset to write.
4. Set **Shot Set** back to **All**.

The function merges the two captures, adds starting points from the new views and trains at reduced learning rates so the rest of the scene is preserved.

## Fallback meshes

`unreal.HarmonyFallbackLibrary` builds [fallback meshes](07_FallbackMesh.md). Its functions block until done.

| Function | What it does |
|---|---|
| `get_default_fallback_settings()` | The default settings, to modify and pass on. |
| `build_fallback_from_cache(cache, settings, output_package_path)` | Builds every tier in `settings` from a cache and writes the assets to `output_package_path`. Returns `"OK: "` followed by a JSON summary, or `"ERROR: ..."`. |
| `build_fallback_options(base_cache, options, settings, base_output_package_path)` | Builds variant options on top of an existing base fallback. |

Unlike the rig's button, `build_fallback_from_cache` does not use the per-user Editor Preferences and does not record the meshes on the splat asset unless you turn on `link_to_splat_asset` in the settings you pass.

## Good to know

- **One job at a time.** A start function refuses while a capture job or training session is running.
- **The level must be saved** and Play In Editor must not be running, the same as for the buttons.
- **Check `get_last_result()`**, not only `is_busy()`. A job that fails also stops being busy.
- **Log categories** for troubleshooting: `LogHarmonyTrainingEditor`, `LogHarmonyCapture` and `LogHarmonyFallback`.
- **These APIs are Alpha.** Function names and arguments may change between releases.
