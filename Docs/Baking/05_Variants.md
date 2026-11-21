# Variants

> **Experimental.** Variant baking works and has produced good results, but it is the least settled part of Harmony. Settings, asset data and behaviour may change between releases, and you should expect rough edges.

A variant bake stores several looks of one scene in a single splat asset: a time of day, a wall finish, a furniture layout. At runtime you switch between them, with an optional fade. This page covers authoring and baking. For switching at runtime, see [Variants (runtime)](../Rendering/09_Variants.md).

## How it works

Harmony captures and trains the scene once as the **base**. For each option it then changes the scene, renders the same camera views again, and trains only what differs. The option is stored as a difference from the base splats: mostly colour changes, plus splats that had to move, change shape, fade out or be added.

That makes an option much smaller and faster to bake than a second splat, as long as it does not change most of the scene:

- A material swap on a few objects changes a small share of the splats and adds little to the asset.
- A whole-scene change costs more. In one test, a golden-hour option on a daytime base changed 70% of the splats and took the asset from 62 MB to 126 MB.

### Sets and options

Options are grouped into **variant sets**. A set is one choice the viewer makes, and its options are the alternatives.

| Variant set | Options |
|---|---|
| `TimeOfDay` | `Day` (base), `GoldenHour` |
| `WallFinish` | `Concrete` (base), `Timber`, `Plaster` |

Sets are treated as independent. Each option is captured against the base of every other set, so two sets of three options need four extra captures, not nine. The trade-off is that interactions between sets are not captured: light that a `Timber` wall bounces onto the floor is only recorded under the base time of day.

Each option costs one more render of every view, a training run, and one more dataset cache on disk.

## Setting up a variant source

A variant source tells the rig which sets and options exist and how to apply them. Choose one in the rig's **Details** panel under **Variants > Variant Source**.

| Variant Source | Use it for |
|---|---|
| **Material Swaps** | Options that only swap materials. Self-contained; needs nothing else in the level. |
| **Level Variant Sets** | Anything Unreal's Variant Manager can switch: lighting, time of day, visibility, transforms, materials. |
| A custom source | Your own configurator. See [Custom sources](#custom-sources). |

### Material Swaps

Add entries under **Sets**:

| Field | What it does |
|---|---|
| **Name** | The variant set's name, for example `Paint`. |
| **Base Option** | The name for what the scene shows as authored. Defaults to `Base`. |
| **Options** | The alternatives. Each has a **Name** and a list of **Swaps**. |

Each swap:

| Field | Default | What it does |
|---|---|---|
| **Actor** | | The actor whose mesh components get the material. |
| **Slot Index** | -1 | The material slot to replace. -1 means every slot that matches **Match Material Name**. |
| **Match Material Name** | Empty | With **Slot Index** -1: only slots whose current material name contains this text. Empty matches every slot. |
| **Material** | | The material to apply. |

**Example.** A showroom wall is one static mesh actor using `M_Wall_Concrete`. To offer a timber finish:

1. Set **Variant Source** to **Material Swaps** and add a set named `WallFinish`. Set **Base Option** to `Concrete`.
2. Add an option named `Timber` with one swap: **Actor** the wall, **Slot Index** -1, **Match Material Name** `Wall`, **Material** `M_Wall_Timber`.

The level stays as authored, showing concrete. During the bake Harmony applies the swap, captures, and puts the original materials back.

### Level Variant Sets

This source uses Unreal's Variant Manager, and it is the route for lighting and time-of-day variants.

1. Create a Level Variant Sets asset, build your variant sets in the Variant Manager as usual, and make sure its Level Variant Sets Actor is in the level.
2. On the rig, set **Variant Source** to **Level Variant Sets** and pick that actor as the **Level Variant Sets Actor**.

Variant sets and variants are matched by their display names. A Variant Manager set `Paint` with variants `Yellow` and `Red` bakes a Harmony variant set `Paint` with options `Yellow` and `Red`. The variant that is active when you start the bake is the base.

Two rules:

- **Every set must have an active variant when you bake.** A set that shows none of its variants is skipped, with a warning in the log. Switch each set to its base variant first.
- **Variants that only run Function Callers are not offered.** A variant must capture at least one property.

At runtime, add a **Harmony Variant Manager Sync** component to keep the splat's variants in step with the Variant Manager. See [Variants (runtime)](../Rendering/09_Variants.md).

### Custom sources

If your project already has a configurator, subclass `HarmonyVariantSource` in Blueprint or C++ and implement two functions: **Get Variant Sets**, which lists the sets, their options and the option each set currently shows, and **Apply Variant**, which makes the scene show one option. Your class then appears in the **Variant Source** dropdown.

## Baking

1. Set up the rig as for a normal capture. See [The Capture Rig](02_CaptureRig.md).
2. Make sure the level shows the base option of every set.
3. Optionally list the sets to bake in **Variants > Variant Sets to Bake**. Empty means all of them.
4. Press **Bake Variants** under **Capture**.

The job runs like [Capture and Train](03_CaptureAndTrain.md), then continues:

1. The base is captured and trained into the **Output Asset**.
2. For each option other than the one its set currently shows: the option is applied, the same views are rendered (`Harmony: rendering 4324 views (WallFinish / Timber, capture 2 of 3)`), and the scene is switched back.
3. The option is baked. A progress dialog titled **Harmony: baking variant WallFinish / Timber** steps through *Finding what the option changes*, *Training the option* and *Baking the option into the asset*. This part blocks the editor.
4. The final message lists what was baked: `base trained, 2 option(s) baked (WallFinish / Timber, WallFinish / Plaster)`.

### Settings

These are in the rig's **Capture** category, under the advanced section.

| Setting | Default | What it does |
|---|---|---|
| **Variant Training Steps** | 25000 | Training steps per option, starting from the base splats. The first fifth trains colour only; then the splats that need to move, reshape or fade for this option are unlocked for the rest. |
| **Variant Threshold** | 0.01 | How much a splat's colour must change (0 to 1) before it is stored in the option. Higher gives a smaller asset and can leave faint traces of the base. |
| **Reuse Base for Variants** | Off | Keeps the existing asset and its last base capture and only captures and bakes the options. See below. |
| **Variant Lighting Probes** | Empty | Per option, whether it keeps its own lighting probe. See [Lighting Capture](06_LightingCapture.md#variants). |

### Adding or redoing options

Turn on **Reuse Base for Variants** to add an option, or redo one, without capturing and training the base again. It needs the trained **Output Asset** and the cache of its base capture.

Only use it while the base look of the scene is unchanged since that capture. Harmony checks that the cached capture has the same number of views as the rig has shots now, and nothing more. If you changed lighting or geometry since, the new options will be baked against a base that no longer matches, and the result will be wrong without any error. When in doubt, turn it off and bake everything.

## Checking the result

Place a Harmony Actor with the asset (see [Viewing the result](03_CaptureAndTrain.md#viewing-the-result)). When the asset has variants, the Harmony component's **Details** panel shows a **Variants** section with a dropdown per set and a **Transition Duration**. The dropdowns cover up to four sets and 32 options per set.

The asset lists what was baked under **Variant Sets** in its own Details panel (read only). **Export to PLY...** on the asset writes the base plus one PLY per option.

## Limitations

- **Experimental.** Expect changes.
- **Training the base again removes every baked variant.** **Capture and Train** and **Retrain From Last Capture** replace the asset's splats, and variants baked for the old splats cannot be kept. Finish the base first, then bake variants.
- **Variants need the exact same camera views as the base.** Do not change the rig, its **Random Seed** or (with **Avoid Geometry** on) large geometry near the cameras between the base capture and the options.
- **Sets are independent.** Combined effects of options from different sets are not captured.
- **Options that change most of the scene** roughly double the asset size each and take the longest to bake.
- **All variants share one floor reflection bake**, taken from the base.
- **Dataset caches add up.** Each option is a cache of its own, and the default **Max Cached Datasets** of 3 will delete older ones. See [Pruning](04_DatasetCache.md#pruning).
- **Baking an option blocks the editor** and can only be cancelled between steps.
