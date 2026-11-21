# Debugging

When a splat looks wrong or runs slowly, Harmony gives you three kinds of tool: viewport **buffer visualisations** that show what the renderer is doing per pixel, the **`stat Harmony`** counters, and a small set of **console variables** for switching parts of the pipeline on and off. This page also explains the most common editor surprise: clicking a splat does not select it.

---

## Why clicking a splat does not select it

When you click in the viewport, Unreal renders a hidden "hit proxy" image to find out which actor is under the cursor. Harmony leaves splats out of that image. A click on a splat therefore lands on whatever mesh is behind it, or on nothing.

This is deliberate. A splat scene usually surrounds the whole level. If it were clickable, almost every click would select the splat instead of the mesh you were aiming at.

To select a Harmony Actor:

- click it in the **Outliner**, or
- give it a folder or a clear label in the Outliner so it is easy to find.

Once it is selected, the transform gizmo works as normal.

The console variable `r.Harmony.Editor.RenderHitProxies` (default 0) makes Harmony run its passes during the picking render. It does not make splats selectable: splats never write the IDs that picking reads, so clicks still pass through them. It is a development switch; leave it at 0.

---

## Buffer visualisations

In a level viewport, open the **View Mode** menu (the button that normally says **Lit**), choose **Buffer Visualization**, and pick one of the **Harmony** entries. Choose **Lit** again to return to the normal view.

| Entry | Shows | Use it to |
|---|---|---|
| **Harmony Scene Coverage Mask** | The mask that separates Unreal content from splat pixels for **Early Masked** tonemapping. See [below](#reading-the-scene-coverage-mask). | Find halos, fringes or flicker where meshes, glass or particles meet splats. |
| **Harmony Separate Translucency** | The translucency Unreal rendered separately. | Check whether a translucent surface is being seen by Harmony at all. |
| **Harmony Scene Depth (opaque + splats)** | Scene depth after Harmony has written splat depth into it. | Check **Write Depth To Scene**, and see what depth of field and TSR receive. |
| **Harmony Splat Overdraw** | A heat map of how many splat fragments were drawn at each pixel. | Find where the GPU time goes. |
| **Harmony Splat Opacity Waste** | A heat map of how many of those fragments were drawn after the pixel was already nearly opaque. | Find overdraw that adds nothing visible. |
| **Harmony Splat Stage Coverage** | Which splat stage drew each pixel. | Understand **Integrated** composition and the translucency split. |
| **Harmony Late Splat Alpha** | The opacity of the late splat stage alone, as greyscale. | See exactly which splats are drawn in front of Unreal content. |

### Reading the overdraw views

Splat rendering cost is mostly overdraw: many semi-transparent splats stacked on the same pixel. **Harmony Splat Overdraw** counts them.

The colour ramp runs from deep blue (few) through blue, cyan, green, yellow and red to magenta and finally white (most). The scale is logarithmic. The count that maps to the hot end is set by `r.Harmony.Debug.SplatOverdraw.HeatmapMax` (default 512). Lower it to see differences in lightly covered areas; raise it to separate extreme hot spots.

**Harmony Splat Opacity Waste** asks a sharper question: of those fragments, how many were probably wasted? Once the splats at a pixel add up to about 99% coverage, further fragments behind them contribute almost nothing.

- Dark blue to blue: the pixel is still building up coverage. This is useful work.
- Blue to yellow: the pixel is approaching 99% coverage.
- Beyond that, the heat map shows the estimated number of redundant fragments, on the same ramp and scale as the overdraw view.

What to do about hot areas:

- Dense foliage, hair-like detail and thick "fog" of faint splats are the usual culprits.
- Raise **Splat Min Alpha** or **Fragment Min Alpha**, or lower **Splat Tail Cutoff**, in [Rendering settings](04_RenderingSettings.md#culling).
- Lower **Splat Resolution**; overdraw cost scales with pixel count.
- Decimate the asset ([Compression and decimation](05_CompressionAndDecimation.md)).
- Use a Preprocess-stage **Harmony Volume** to remove floaters near the camera path ([Placing splats](03_PlacingSplats.md#harmony-volume-actor)).

### Reading Stage Coverage

In **Integrated** composition, splats are drawn in an early stage (behind Unreal content) and a late stage (in front of it). **Harmony Splat Stage Coverage** colours each pixel by which stage contributed:

| Colour | Meaning |
|---|---|
| Red | Early stage only |
| Green | Late stage only |
| Yellow | Both |
| Black | No splats |

Expect green or yellow where splats overlap Unreal geometry or sit in front of glass that writes Custom Depth, and red everywhere else. If a splat that should be in front of a mesh is missing, and that area is red, the late stage is not picking it up: check the component's **Depth Offset** settings, and for glass the [translucency setup](04_RenderingSettings.md#translucency-in-front-of-splats).

### Reading the Scene Coverage Mask

**Harmony Scene Coverage Mask** shows three channels at once. Red is the mask itself: red where a pixel is Unreal content that gets tonemapped, black where it is splat that keeps its captured colours. Green adds opaque geometry with no splat over it, and blue adds translucency. So, with the default settings:

| Colour | Meaning |
|---|---|
| Black | Splat, kept as captured |
| Red | Unreal content with no depth, such as sky or fog |
| Yellow | Opaque Unreal geometry |
| Magenta | Translucency over a splat; the pixel is tonemapped as a whole |
| White | Translucency over opaque geometry |

Fringes show up at the edge between black and the other colours around your meshes. Tune that edge with **Mask Expansion (px)**, **Feather Radius (px)** and **Use Mask History**.

The view works out the mask whenever it is selected, so it shows something in every mode, but only **Early Masked** tonemapping uses the mask. In the other modes it only shows what Early Masked would do.

---

## stat Harmony

Type `stat Harmony` in the console (the `` ` `` key) to toggle an on-screen panel. Type it again to hide it.

| Counter | Meaning |
|---|---|
| **Sort Device (1=GPU, 2=CPU)** | Which sorter the last splat draw used. With **Sorting** on **Auto** this shows what Auto chose. |
| **CPU Sort (ms)** | Time the CPU sorter took on its worker thread. |
| **CPU Sort Wait (ms)** | Time the renderer waited for a CPU sort result. |
| **CPU Sort Splats** | Number of splats the CPU sorter handled. |
| **CPU Sort Fallbacks** | Times a CPU sort was not ready and the GPU sorter was used instead. A steadily climbing number means the CPU cannot keep up; set **Sorting** to **GPU**. |
| **Splat Resolution (%)** | The splat layer's current resolution. Watch this to see **Dynamic Splat Resolution** at work. |

For GPU timings, Harmony's passes are labelled with names starting `Harmony` (for example `Harmony PreprocessAndSort`, `Harmony EarlySplats`, `Harmony LateSplats`, `Harmony SceneCoverageAndTonemap`) in Unreal's GPU profiling tools such as `ProfileGPU` and Unreal Insights.

---

## Useful console variables

Most Harmony project settings have an `r.Harmony.*` console variable behind them, and there are many more for development. These are the ones worth knowing. A value set in the console lasts until the editor or game closes; it is not saved.

### Isolating a problem

| Variable | Default | What it does |
|---|---|---|
| `r.Harmony.Feature.Compose.Mode` | 0 | The **Composition** setting: 0 Integrated, 1 Background Plate, 2 Splats Only. Handy for quick A/B comparisons. |
| `r.Harmony.Debug.Splats.StageSelect` | 0 | 0 draws both splat stages, 1 the early stage only, 2 the late stage only. |
| `r.Harmony.Feature.Occlusion.DepthReject` | 1 | 0 stops splats being hidden behind Unreal opaque geometry. If a splat reappears, depth or **Depth Offset** was hiding it. |
| `r.Harmony.Feature.SceneCapture.Enable` | 1 | 0 stops splats rendering into Scene Capture components. |
| `r.Harmony.Reflections` | 1 | 0 switches off floor reflections everywhere. |
| `r.Harmony.Feature.AutoExposure.MaskSplats` | 1 | 0 lets splat pixels influence auto exposure again (Early Masked and Tonemap Compensation modes). |

### Performance experiments

| Variable | Default | What it does |
|---|---|---|
| `r.Harmony.Feature.Sort.Device` | 0 | The **Sorting** setting: 0 Auto, 1 GPU, 2 CPU. |
| `r.Harmony.Tuning.SplatResolution.Percentage` | 100 | The **Splat Resolution** setting. |
| `r.Harmony.Debug.DynamicResolution.ForcePercentage` | 0 | Forces the splat resolution to this percentage whether or not the camera is moving. 0 is off. Use it to judge how low you can go. |
| `r.Harmony.Debug.Benchmark.SHDegreeOverride` | -1 | Forces the spherical harmonic degree used for colour: 0 to 3, or -1 to use the asset's. 0 shows the cost and visual contribution of view-dependent colour. |
| `r.Harmony.Debug.SplatOverdraw.HeatmapMax` | 512 | Scale of the overdraw and opacity waste heat maps. |

### Logging

| Variable | What it does |
|---|---|
| `r.Harmony.Profile.Uploads 1` | Logs how long it takes to build and upload splat data to the GPU, whenever that happens. |
| `r.Harmony.Profile.CpuSort N` | Logs CPU sort timings and fallbacks every N frames. |
| `r.Harmony.Profile.DynamicResolution N` | Logs Dynamic Splat Resolution decisions every N frames. |

Set any of them back to 0 to stop the logging.

---

## Common problems

| Symptom | Things to check |
|---|---|
| Nothing draws | **Splat Data Asset** is set. The actor is not hidden. **Edit > Project Settings > Plugins > Harmony > Enabled** is on. The camera is not inside the **Camera Near Fade** distance of everything. |
| Splat is visible in the editor but washed out, too dark or too contrasty | **Tonemapping Mode**. See [Rendering settings](04_RenderingSettings.md#tonemapping). |
| Bright or dark fringe around meshes in front of the splat | **Harmony Scene Coverage Mask** view, then **Mask Expansion (px)** and **Feather Radius (px)**. |
| A mesh on splat ground looks sunk or has smeared feet | **Depth Offset** on the Harmony component. See [Placing splats](03_PlacingSplats.md#depth-offset). |
| Splats in front of a window are drawn behind the glass | The glass must write Custom Depth. See [Translucency in front of splats](04_RenderingSettings.md#translucency-in-front-of-splats). |
| Depth of field blurs splats wrongly | **Write Depth To Scene** on the component, **Depth Write Coverage Threshold** in settings, and the **Harmony Scene Depth (opaque + splats)** view. |
| Slow when the camera moves, fast when still | Normal: a still view is cached. Lower **Splat Resolution** or enable **Dynamic Splat Resolution**. |
| Hitch when a splat first appears or a volume is toggled | Splat data is being uploaded to the GPU. `r.Harmony.Profile.Uploads 1` logs how long it takes. |
| Lit meshes do not match the splat's brightness | **Lighting Status** on the Harmony Lighting component. See [Lighting and shadows](10_LightingAndShadows.md). |
| No shadow under an object | **Shadow Status** on the Shadow Catcher. The object must be a caster. |
| Variants switch instantly instead of fading | The asset must be Compressed. See [Variants](09_Variants.md#animated-transitions). |
| Cannot click the splat | Select it in the Outliner. See the top of this page. |

---

## Good to know

- **Console variables are not a stable interface.** Names and defaults may change between releases. Do not build shipping logic on them; use the project settings and the Blueprint functions.
- **Buffer visualisations are diagnostic views.** Some of them do extra rendering work while selected, so do not measure performance with one active.
- **Non-realtime viewports.** If a viewport's **Realtime** option is off, Harmony's views only update when the viewport redraws.

---

## Next steps

- [Rendering settings](04_RenderingSettings.md)
- [Low-end hardware](../Guides/03_LowEndHardware.md)
