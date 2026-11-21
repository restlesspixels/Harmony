// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "HarmonyTrainingEditorLibrary.generated.h"

/**
 * Python / Blueprint entry points of the HarmonyTraining asset writer (CONTRACT_P4.md P1), used by the headless test
 * Scripts/EnvGen/splat_asset_test.py (SplatAssetTest.ps1). Every function runs on the game thread and blocks.
 * Reports are single-line JSON objects (Python: json.loads).
 */
UCLASS()
class HARMONYTRAININGEDITOR_API UHarmonyTrainingEditorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * HarmonyTraining::LoadPly(PlyPath) -> HarmonyTrainingEditor::WriteHarmonyAsset(LongPackageName, ..., bCompress)
	 * (load-then-overwrite, non-finite rows dropped, package saved).
	 * Returns {"ok", "error", "asset", "num_points", "dropped", "overwrote", "encoding"}.
	 */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	static FString WriteAssetFromPly(const FString& PlyPath, const FString& LongPackageName, bool bCompress);

	/**
	 * HarmonyTraining::LoadPly(InPlyPath) -> HarmonyTraining::WritePly(OutPlyPath): the PLY the trainer would write for
	 * the splats WriteAssetFromPly converts (exp/log and sigmoid/logit round trip). Returns "" or the error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	static FString RewritePly(const FString& InPlyPath, const FString& OutPlyPath);

	/**
	 * Field-by-field comparison of two UHarmonyDataAssets' FHarmonyRawData (object paths /Pkg/Name.Name), Expected
	 * first. With bDropNonFiniteFromExpected, Expected's non-finite rows are removed first (as WriteHarmonyAsset does).
	 * Returns {"ok", "error", "dropped_from_expected", "meta": {name: {"e", "a", "equal"}},
	 *          "arrays": {name: {"count_e", "count_a", "mismatch" (elements not bitwise equal), "first", "max_abs",
	 *          ["max_rel"]}}, "meta_mismatch": [...], "array_mismatch": [...]}.
	 * Covariance "max_rel" = max over splats of max|a - e| / trace(e) (e: the expected 3x3 covariance, cm^2).
	 * Expanded arrays are compared when both are Expanded, compressed arrays when both are Compressed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	static FString CompareAssetRawData(const FString& ExpectedObjectPath, const FString& ActualObjectPath, bool bDropNonFiniteFromExpected);

	/**
	 * Sets the Harmony import settings UHarmonyPlyFactory reads (Project Settings > Plugins > Harmony Import) in memory
	 * only (not saved to config), so a headless test controls compression and decimation of its imports.
	 */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	static void SetHarmonyImportSettings(bool bImportCompressed, float DecimationTargetRatio);

	/**
	 * Phase 6: UHarmonyTrainingLibrary::TrainSplat (blocking, headless) on a cached capture dataset (folder or name under
	 * the project's DatasetCacheRoot): Init "colmap" from the cache's points3D, the cache's images and eval_interval,
	 * Steps 0 / OptionsJson keys = the project defaults (UHarmonyTrainingSettings) underneath. OutAsset (optional long
	 * package name) is written through the asset writer hook. Returns TrainSplat's "OK: ..." / "ERROR: ..." line.
	 */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	static FString TrainFromCache(const FString& CacheDir, int32 Steps, const FString& OutPly, const FString& OptionsJson, const FString& OutAsset);

	/** Phase 9: writes a Harmony asset's splats as a trainer PLY (ConvertHarmonyAssetToSplats; lossy for compressed
	    assets). Returns "" or the error. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	static FString ExportAssetToPly(const FString& AssetObjectPath, const FString& PlyPath);

	/**
	 * Phase 9 refinement (headless, blocking): fine-tunes an existing model on its base capture plus a refinement
	 * capture (e.g. the capture rig's custom views, ShotSet CustomViewsOnly).
	 * - Init: a PLY / state dir, or a Harmony asset's object path (/Pkg/Name.Name, converted; no extra file needed).
	 * - The two caches are merged (refinement images renamed zz_refine_<k>_<name>, repeated RefineRepeat times so the few
	 *   new views are drawn as often as they need); masks / depth / cull options follow the caches; scene_scale = the
	 *   base cache's.
	 * - The refinement capture's sampler points are appended as new splats (opacity init_opacity, default 0.1).
	 * - Defaults (OptionsJson keys win): strategy mcmc, cap_max = init + new points, refine_start_iter 100,
	 *   refine_stop_iter 0.8 Steps, and every learning rate at 20 % of a first training's (means 1.6e-5 -> 1.6e-7, SH dc
	 *   5e-4, rest 2.5e-5, opacity 0.01, scale 1e-3, rotation 2e-4): +7.8 dB on the refined views, +0.13 dB elsewhere.
	 * Returns TrainSplat's "OK: ..." / "ERROR: ..." line. Per-view PSNR in <OutPly base>_train.json (image_name).
	 */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	static FString RefineFromCaches(const FString& Init, const FString& BaseCache, const FString& RefineCache, int32 Steps, const FString& OutPly,
		const FString& OptionsJson, const FString& OutAsset, int32 RefineRepeat = 16);

	/** Phase 6: cached capture datasets, newest first. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	static TArray<FString> ListDatasetCaches();

	// ---- P10 configurable variants (HarmonyTrainingVariants.h) ----

	/** Change masks of a variant capture against the base capture (same poses): masks_variant_<ds> + harmony_variant.json
	    in the variant cache. Threshold: per-channel difference (0..1); DilatePx; OutsideWeight: loss weight of unchanged
	    pixels in changed views. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training|Variants")
	static FString BuildVariantMasks(const FString& BaseCache, const FString& VariantCache, float Threshold = 0.03f, int32 DilatePx = 6,
		float OutsideWeight = 0.15f, int32 BlurPx = 1, int32 ErodePx = 1);

	/** Trains a variant option from the base asset (frozen geometry, SH only) on the variant cache's changed views. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training|Variants")
	static FString TrainVariant(const FString& BaseAsset, const FString& VariantCache, int32 Steps, const FString& OutPly, const FString& OptionsJson);

	/** Writes VariantPly's appearance delta (points whose colour moved more than Threshold) into the base asset as
	    SetName / OptionName and saves the asset. BaseOptionName names what the base splats show (optional). */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training|Variants")
	static FString BakeVariantOption(const FString& BaseAsset, const FString& VariantPly, FName SetName, FName OptionName, FName BaseOptionName,
		float Threshold = 0.01f, const FString& BakeInfo = TEXT(""));

	/** The asset with one option applied, as a PLY. Returns "" or the error. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training|Variants")
	static FString ExportVariantPly(const FString& Asset, FName SetName, FName OptionName, const FString& OutPly);

	/** "Export to PLY" (the asset context menu): <OutDir>/<Name>.ply plus <Name>_<Set>_<Option>.ply per baked option.
	    Returns "OK: <n> file(s): ..." or "ERROR: ...". */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training|Variants")
	static FString ExportAssetToPlyFiles(const FString& Asset, const FString& OutDir);

	/** 16 Nov 2026: the capture's point sampler (FColmapPointSampler, as the MRQ capture runs it) re-run offline on a cache's
	    saved frames with another TargetPoints: colour from the cache's images, depth from DepthDir (<stem>.f16, metres, 0 =
	    none; empty = the cache's depth channel); where that is 0, FillDepthDir (any resolution, nearest) fills in (e.g. the
	    opaque depth seen through glass, as the capture's replay sees it). Writes cameras.txt / images.txt / points3D.txt /
	    validation_report.txt into OutSparseDir. Returns "OK: ..." or "ERROR: ...". */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	static FString ResampleCacheInit(const FString& Cache, const FString& DepthDir, const FString& FillDepthDir, int32 TargetPoints, const FString& OutSparseDir);

	/** Val PSNR of a splat (PLY, state dir or asset path) on a cache; bChangedViewsOnly: the views in harmony_variant.json. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training|Variants")
	static FString EvaluateSplatOnCache(const FString& Splat, const FString& Cache, bool bChangedViewsOnly, const FString& EvalOutDir);

	// ---- P11 variants: dynamic locking (HarmonyTrainingVariants.h) ----

	/** TrainVariant with dynamic locking and option-only splats; OptionsJson: see HarmonyTrainingEditor::TrainVariantDynamic. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training|Variants")
	static FString TrainVariantDynamic(const FString& BaseAsset, const FString& VariantCache, const FString& OutPly, const FString& OptionsJson);

	/** Renders a splat for some views of a cache ("every:N" or a comma list of frames) into OutDir as .f32 files. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training|Variants")
	static FString RenderSplatViews(const FString& Splat, const FString& Cache, const FString& Views, const FString& OutDir);

	/** Bakes a dynamically trained variant into the asset as a sectioned, compressed option; SettingsJson: see
	    HarmonyTrainingEditor::BakeVariantOptionDynamic. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training|Variants")
	static FString BakeVariantOptionDynamic(const FString& BaseAsset, const FString& VariantPly, FName SetName, FName OptionName, FName BaseOptionName,
		const FString& SettingsJson);

	/** Phase 13: captures a lighting probe of the editor world at LocationCm (what a Harmony capture does at the rig's
	    centre) into OutDir/lighting_probe.hdr + .json. Capture rigs and what they exclude are hidden. bMeasureNoiseFloor:
	    captured twice, the difference is stored as the noise floor (a job's base capture does). ShadowStandBelowCm >= 0
	    (Phase 14): the ground that far below LocationCm is the stand; its heightfield and the sun's share of its light
	    are captured too. FloorReflection (Phase 16, with the stand; 0 Auto, 1 Always, 2 Never): the floor's material is
	    measured there and baked (reflection_material.png) as the capture rig would. Returns "OK: ..." or "ERROR: ...". */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training|Lighting")
	static FString CaptureLightingProbe(FVector LocationCm, int32 LongLatWidth, const FString& OutDir, float CaptureEV100 = -1000.0f,
		bool bMeasureNoiseFloor = false, float ShadowStandBelowCm = -1.0f, int32 FloorReflection = 0);

	/** Phase 16: the floor reflection measurement alone, in the editor world: the floor's material around StandCm (the
	    ground under the rig's origin) from above, and what a capture with Floor Reflection = Mode (0 Auto, 1 Always,
	    2 Never) would decide. OutDir not empty and the decision is to bake: reflection_material.png is written there.
	    Returns "BAKE: ..." or "SKIP: ..." with the numbers. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training|Lighting")
	static FString MeasureFloorReflection(FVector StandCm, const FString& OutDir, int32 Mode = 0);

	/** Phase 13: what Bake Variants decides for an option whose capture is in OptionDir against the base capture in
	    BaseDir (Store: 0 Auto, 1 Always Store, 2 Never Store): "STORE: ..." or "SKIP: ...", with the numbers. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training|Lighting")
	static FString DecideVariantLightingProbe(const FString& BaseDir, const FString& OptionDir, int32 Store = 0);

	/** Phase 13: removes the asset's lighting probe (SetName none) or one option's, and saves the asset. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training|Lighting")
	static FString ClearLightingProbe(const FString& Asset, FName SetName, FName OptionName);

	/** Phase 13: imports Dir/lighting_probe.hdr into the Harmony asset as its lighting probe (SetName none) or as the
	    probe of one variant option, and saves the asset. Returns "OK: ..." or "ERROR: ...". */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training|Lighting")
	static FString SetLightingProbeFromCache(const FString& Asset, FName SetName, FName OptionName, const FString& Dir);

	/** One line per option of the asset: its sections and stored size (HarmonyVariant::Describe). */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training|Variants")
	static FString DescribeVariantOptions(const FString& Asset);
};
