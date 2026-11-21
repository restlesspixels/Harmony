// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UHarmonyDataAsset;

/**
 * HarmonyTraining P10: configurable variants (notes/phase10.md).
 *
 * A variant option is baked as a sparse appearance delta on a base Harmony asset: the same splats (position, rotation,
 * scale, opacity frozen), a new SH for the points whose colour the option changes. The pipeline, per option:
 *   1. capture the base configuration and the option with the same rig poses (two dataset caches);
 *   2. BuildVariantMasks: per view, the pixels where the option's render differs from the base render (plus a dilation)
 *      get full loss weight, the rest a small anchor weight; views with no change are left out (harmony_variant.json);
 *   3. TrainVariant: init = the base ASSET's own splats (so row i is asset point i), geometry learning rates 0, no
 *      densification, no pruning, only the SH learns, on the changed views;
 *   4. BakeVariantOption: the points whose colour moved more than a threshold become the option's delta in the asset
 *      (UHarmonyDataAsset::VariantSets), which UHarmonyComponent::SetVariant shows at runtime.
 * Bounce light comes for free: a red wall that tints the floor shows up in the diff, so the floor splats get a delta too.
 */
namespace HarmonyTrainingEditor
{
	struct FHarmonyVariantMaskSettings
	{
		float Threshold = 0.03f;      // per-channel absolute difference (0..1) above which a pixel changed
		int32 BlurPx = 1;             // box blur radius applied to both images first (render noise is per pixel)
		int32 ErodePx = 1;            // shrink the changed region first (drops isolated noisy pixels), then dilate
		int32 DilatePx = 6;           // grow the changed region by this many pixels (box)
		float OutsideWeight = 0.15f;  // loss weight of unchanged pixels in changed views (anchors boundary splats)
		float MinChangedFraction = 0.0005f;   // views with fewer changed pixels are left out
	};

	/** Writes <VariantCache>/masks_variant_<ds>/<frame>.png and <VariantCache>/harmony_variant.json. Returns a summary line, or
	    "ERROR: ..." */
	HARMONYTRAININGEDITOR_API FString BuildVariantMasks(const FString& BaseCache, const FString& VariantCache, const FHarmonyVariantMaskSettings& Settings);

	/** Trains the option from the base asset on the variant cache's changed views (see the header comment). OutPly holds the
	    asset's point count, in the asset's point order. Returns the trainer's "OK: ..." / "ERROR: ..." line. */
	HARMONYTRAININGEDITOR_API FString TrainVariant(const FString& BaseAssetPath, const FString& VariantCache, int32 Steps, const FString& OutPly,
		const FString& OptionsJson);

	/** Delta of VariantPly against the base asset (points whose colour moved more than Threshold, 0..1) written into the
	    asset's variant set SetName as option OptionName (replacing an option of that name), then the asset is saved. */
	HARMONYTRAININGEDITOR_API FString BakeVariantOption(const FString& BaseAssetPath, const FString& VariantPly, FName SetName, FName OptionName,
		FName BaseOptionName, float Threshold, const FString& BakeInfo);

	/** The base asset with one option applied, as a PLY (for evaluation). Returns "" or the error. */
	HARMONYTRAININGEDITOR_API FString ExportVariantPly(const FString& AssetPath, FName SetName, FName OptionName, const FString& OutPly);

	/** "Export to PLY": <OutDir>/<AssetName>.ply (the base look) plus <AssetName>_<Set>_<Option>.ply per baked option
	    (other sets on their base), each as the renderer shows it. OutFiles: the files written. Returns "" or the errors
	    ("; "-joined; files that worked are still written). */
	HARMONYTRAININGEDITOR_API FString ExportAssetPlys(const UHarmonyDataAsset& Asset, const FString& OutDir, TArray<FString>& OutFiles);

	/** Renders Splat (a PLY / state dir, or an asset path) against the cache's val views (with the capture's own masks and
	    background) and returns the trainer's result line (val PSNR); no learning. bChangedViewsOnly uses the views listed
	    in the cache's harmony_variant.json. EvalOutDir (optional) receives the val renders. */
	HARMONYTRAININGEDITOR_API FString EvaluateSplatOnCache(const FString& Splat, const FString& Cache, bool bChangedViewsOnly, const FString& EvalOutDir);

	/**
	 * P11 (notes/phase11_variants.md): TrainVariant with dynamic locking. Geometry is no longer frozen: per splat, opacity,
	 * position and shape (rotation + scale) are three lock groups. A locked group stays exactly on the base (and stores
	 * nothing); groups are unlocked where that buys the most, and the option may get splats of its own where the base's
	 * cannot draw what the variant needs.
	 * OptionsJson (all optional):
	 *   mode "budget" (default): a first run learns SH only (warmup steps, 5000) and, over its last pass through the train
	 *     views, sums the loss gradient of every pinned group (the first-order gain of freeing it). Then `rounds` (1) runs
	 *     of round_steps (20000) each (the steps after the unlock matter more than the budget): the still-locked groups with the highest gain per stored byte (cost_position 6,
	 *     cost_shape 14, cost_opacity 1, cost_size 1; weight_* scale a group's gain, 0 = never unlock it) are freed until
	 *     `budget` (2.0: geometry bytes per base row, on average) is spent, budget_first (0.6) of what is left per round,
	 *     and the gains are measured again at the end of each round. "Size" is a fourth, cheap group: one isotropic scale
	 *     factor on a splat whose shape otherwise stays the base's.
	 *   mode "prox": one run of steps (15000): warmup (steps / 5) SH only, discover (2 steps / 5) with a proximal
	 *     group-lasso step after every Adam step (kappa_position / kappa_shape / kappa_opacity, 0.1: a group leaves the
	 *     base only when Adam's normalised pull exceeds kappa), then the groups still on the base are locked for good.
	 *   rows ("auto": every row when most pixels changed, else the rows on changed pixels; "all"; "changed"),
	 *   add_fraction (0: no option-only splats; else that fraction of the rows is cloned where gradient pressure remains
	 *     and trained add_steps (6000) more steps; add_scale 0.6, add_opacity 0.2, add_jitter 0.5, add_min_opacity 0.02),
	 *   seed, and "train": trainer options on top of everything (learning rates etc.).
	 * OutPly: rows [0, asset points) are the asset's points in order, then the added rows. Next to it:
	 * <base>_variant_rows.bin, one byte per row (bit 0 / 1 / 2: position / shape / opacity is locked = the base's; bit 7:
	 * the size is free although the shape is locked), and <base>_variant_sens.f32, 4 floats per row (the loss's sensitivity
	 * to the row's SH: DC, band 1, band 2, band 3), which BakeVariantOptionDynamic uses to weight the compression.
	 * Returns the trainer's "OK: ..." / "ERROR: ..." line (the last stage's, with a summary).
	 */
	HARMONYTRAININGEDITOR_API FString TrainVariantDynamic(const FString& BaseAssetPath, const FString& VariantCache, const FString& OutPly,
		const FString& OptionsJson);

	/**
	 * Renders Splat (PLY / state dir / asset path) for some views of a cache into OutDir as <frame>.f32 (W*H float4:
	 * premultiplied RGB over black, transmittance), train or val views alike, loading only those views. Views: "every:N"
	 * (sorted position k with k % N == 0) or a comma list of frame names / numbers ("421,1263" = frame_000421, ...). No
	 * masks, no background compositing. Returns the trainer's result line (its PSNR is over these views, unmasked).
	 */
	HARMONYTRAININGEDITOR_API FString RenderSplatViews(const FString& Splat, const FString& Cache, const FString& Views, const FString& OutDir);

	/**
	 * P11: bakes a variant trained by TrainVariantDynamic (or TrainVariant) into the asset as a sectioned option
	 * (FHarmonySplatVariantOption, FormatVersion 2) and saves the asset. VariantPly: rows [0, asset points) are the asset's
	 * points in order, further rows are splats only the option has. Stored, each only for the points it concerns:
	 * - appearance: the SH delta of the points whose colour moved more than sh_threshold (0.01, as P10; 0 = every point).
	 *   DC as int8; the view-dependent part by sh_rest: "codebook" (default: per SH band a k-means codebook of
	 *   codebook_entries (4096) delta vectors, codebook_stages (1) residual stages, codebook_iterations (10)),
	 *   "luma_chroma" (per band a codebook of the delta's grey part, codebook_entries / codebook_stages, and one of its
	 *   colour part, chroma_entries (256) / chroma_stages (1; 0 = dropped)), "int8" (45 bytes per point) or "none" (the
	 *   base's). Codebooks are weighted by "sens" (default <ply>_variant_sens.f32: how strongly the loss answers to each
	 *   point's SH) and the sh_exact_fraction (0.02) of the points where their error matters most keep their exact delta;
	 * - opacity / position / shape of the groups the training unlocked ("rows": the lock-bit file, default
	 *   <ply>_variant_rows.bin; without it every group is compared) and that really moved (opacity: another byte value;
	 *   position: more than position_min (0.01) of the splat's size; shape: more than shape_min (0.01) relative change);
	 * - the added rows with opacity >= added_min_opacity (1 / 255), compressed like the asset.
	 * "info": free text for BakeInfo. Returns "OK: ..." with the sizes per section, or "ERROR: ...".
	 */
	HARMONYTRAININGEDITOR_API FString BakeVariantOptionDynamic(const FString& BaseAssetPath, const FString& VariantPly, FName SetName, FName OptionName,
		FName BaseOptionName, const FString& SettingsJson);
}
