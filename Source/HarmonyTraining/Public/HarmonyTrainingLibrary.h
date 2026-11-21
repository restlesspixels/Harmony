// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "HarmonyTrainingLibrary.generated.h"

UCLASS()
class HARMONYTRAINING_API UHarmonyTrainingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Renders the given image indices (into the name-sorted COLMAP image list) with the RDG forward rasterizer and
	    writes <OutDir>/<frame>.f32 (raw float32 W*H*4 = PixelState) and <OutDir>/<frame>.png (16-bit RGB,
	    round(65535*clamp(rgb,0,1)), like VkSplat's val renders) plus <OutDir>/summary.json (per frame: name, W, H,
	    intersections total/clamped/overflow, GPU ms if easy). Returns a one-line summary or an error string. */
	UFUNCTION(BlueprintCallable, Category="Harmony|Training")
	static FString RenderParityViews(const FString& PlyPath, const FString& ColmapSparseDir, int32 Downscale,
	                                 const TArray<int32>& ImageIndices, const FString& OutDir, int32 IntersectionCapacity = 16777216);

	/** G1 harness: for every case dir under CasesRoot (or CasesRoot itself if it has meta.json) runs stage1/2/3/chain as
	    CONTRACT_P2.md section 5 and writes <OutRoot>/<case>/... Returns "OK: ..." or "ERROR: ...". Blocking, game thread.
	    Repeats > 1 adds rK_ copies (K = 1..Repeats-1) of the stage2 and chain outputs. Needs an SM6 RHI.
	    Implemented in HarmonyTrainingParity.cpp. */
	UFUNCTION(BlueprintCallable, Category="Harmony|Training")
	static FString RunGradientParity(const FString& CasesRoot, const FString& OutRoot, int32 Repeats = 1);

	/** G2: trains a fixed-count splat set. Init = state dir or PLY. ImagesDir = folder of the (already downscaled) images
	    named as in images.txt; Downscale is inferred from a trailing "_<n>" in the folder name (images_2 -> 2), else 1.
	    OptionsJson keys (all optional): max_steps (= Steps), eval_interval 8, ssim_lambda 0.2, means_lr 1.6e-4,
	    means_lr_final 1.6e-6, features_dc_lr 0.0025, features_rest_lr 0.000125, opacities_lr 0.05, scales_lr 0.005,
	    quats_lr 0.001, scale_reg 0.01, opacity_reg 0.01, scene_scale (default ComputeSceneScale over ALL cameras),
	    order_file (order.i32; default generator with seed), seed 0, intersection_capacity 16777216, steps_per_chunk 16,
	    log_every 500, out_state_dir, eval (true: render every val image at the end and report mean/min PSNR on
	    clamp(C,0,1) vs ref/255, same as train_splat_vksplat.py), eval_out_dir (optional .f32 val renders).
	    Phase 3 (CONTRACT_P3.md section 4): strategy "default" (fixed N) or "mcmc"; cap_max 1000000, refine_start_iter
	    500, refine_stop_iter 2/3 max_steps + 1 (16 Nov 2026; max(25000, 0.9 max_steps) when opacity_reg_until is given),
	    refine_every 100, grow_factor 1.05, min_opacity 0.005, noise_lr 5e5, mcmc_seed 0,
	    max_scale 0 (off), noise_max_disp 0 (off). 16 Nov 2026: reloc_score "opacity_error" (relocation sources by opacity x
	    screen-gradient error; "opacity" = VkSplat, "error"), reloc_jitter 0, opacity_reg_until auto = refine_stop_iter - 1
	    (no opacity regulariser after the last relocation, so the export keeps ~96 % of cap_max; 0 = never stop). Init "colmap" (case-insensitive) builds the native init from
	    <ColmapSparseDir>/points3D.{bin,txt} with init_scale 0.1, init_opacity 0.5, rot_seed 42.
	    Phase 5: normalize_world true (scale_reg * s and, for mcmc, noise_lr * s^2 with s = 1.1 / scene_scale, the frame
	    gsplat tuned them in; +1 dB on Day_orbit); false = parity mode (VkSplat with normalize_world=False).
	    Anisotropy regulariser (needle splats; PhysGaussian's scale-ratio loss): aniso_lambda 0 (off: training is
	    bit-identical), aniso_ratio 4, aniso_mode "max_mid" (or "max_min"). Adds aniso_lambda * mean over the N splats of
	    max(s_max / s_b, aniso_ratio) - aniso_ratio to the loss, s_b = the middle scale ("max_mid": penalises needles, not
	    the flat discs ground is made of) or the smallest ("max_min": also flattens discs, e.g. init_orient "normals"
	    discs at init_flatten 0.1 have ratio 10). Gradient +-lambda/N * s_max/s_b on ln s_max, ln s_b, dense (every
	    splat, every step, like scale_reg); scale-free, so not touched by normalize_world. Try 0.1..1 with ratio 3..10.
	    antialiased true (false = the old training): Mip-Splatting's 2D filter (VkSplat ANTIALIASE_MODE 1), each splat's opacity x
	    sqrt(det(cov2d) / det(cov2d + 0.3)) in training and eval, so splats thinner than a pixel fade instead of growing:
	    the model holds up viewed smaller than it was trained. Draw such splats with the same compensation.
	    Phase 4 (CONTRACT_P4.md P0): out_asset (a long package name, e.g. /Env_Showroom/Splats/SPL_Day) also writes a
	    Harmony asset after the PLY through HarmonyTraining::GetAssetWriterHook(), registered by the HarmonyTrainingEditor
	    module ("ERROR: out_asset needs HarmonyTrainingEditor" without it).
	    Returns "OK: steps=.., N=.., time=..s, ms/step=.., final loss=.., val PSNR mean=.. min=.., overflow steps=..,
	    final N=.., dead/relocated total=.., nonfinite rows=.. -> <ply>" or "ERROR: ...".
	    = HarmonyTraining::LoadColmapDataset + HarmonyTraining::TrainSplatFromDataset (HarmonyTrainingDataset.h).
	    Implemented in HarmonyTrainingTrainer.cpp (driver) and HarmonyTrainingTrainerCore.cpp (core). */
	UFUNCTION(BlueprintCallable, Category="Harmony|Training")
	static FString TrainSplat(const FString& Init, const FString& ColmapSparseDir, const FString& ImagesDir, int32 Steps,
	                          const FString& OutPly, const FString& OptionsJson = TEXT(""));

	/** G3a harness (CONTRACT_P3.md section 5): for every case dir under CasesRoot (or CasesRoot itself if it has
	    meta.json: num_splats, capacity, num_add, step_seed, refine, noise, min_opacity, max_scale, noise_scale,
	    noise_max_disp; plus state/ and adam/) runs exactly one AddMCMCPostBackwardPasses on the injected state and writes
	    <OutRoot>/<case>/{state/, adam/, reloc_index.u32, reloc_counts.u32, add_index.u32, add_counts.u32, stats.u32,
	    summary.json}; state/ and adam/ hold num_splats + num_add rows. Each case runs twice; the second run must be
	    bitwise identical (K8). Returns "OK: ..." or "ERROR: ...". Blocking, game thread, SM6 RHI.
	    Implemented in HarmonyTrainingMCMCTest.cpp. */
	UFUNCTION(BlueprintCallable, Category="Harmony|Training")
	static FString RunMCMCStep(const FString& CasesRoot, const FString& OutRoot);

	/** G3b: builds the native init from <SparseDir>/points3D.{bin,txt} (CONTRACT_P3 P9) and writes a state dir.
	    OptionsJson keys: init_scale 0.1, init_opacity 0.5, rot_seed 42. Returns "OK: N=.., knn ms=.." or "ERROR: ...".
	    Implemented in HarmonyTrainingInit.cpp. */
	UFUNCTION(BlueprintCallable, Category="Harmony|Training")
	static FString BuildColmapInit(const FString& SparseDir, const FString& OutStateDir, const FString& OptionsJson = TEXT(""));
};
