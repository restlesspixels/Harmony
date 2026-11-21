// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HarmonyTrainingTypes.h"

/**
 * A training dataset in memory (CONTRACT_P4.md section 2, P0).
 * Producers: HarmonyTraining::LoadColmapDataset (COLMAP text model + image folder; what TrainSplat uses) and
 * HarmonyTraining::AddDatasetImage (in-memory producers, e.g. the Phase 4 MRQ output). Consumer: TrainSplatFromDataset
 * (and the trainer core). Not thread-safe: fill it from one thread, then treat it as immutable while a run uses it.
 */
struct HARMONYTRAINING_API FHarmonyTrainingDataset
{
	/** Every camera, already downscaled, sorted by ImageName in case-sensitive byte order (as LoadColmapText). */
	TArray<FHarmonyTrainingCamera> Cameras;

	/** Per sorted camera index: Width*Height RGBA8 pixels in a uint32, R in the low byte, alpha 255 (CONTRACT_P2 D10).
	    Empty when not loaded (LoadColmapDataset leaves the val images empty when bLoadValImages is false). */
	TArray<TArray<uint32>> Images;

	/** Sorted camera indices of the train and val images: sorted position k is val when EvalInterval > 0 and
	    k % EvalInterval == 0 (gs_trainer.cpp:408), train otherwise. Ascending. Rebuilt by RebuildSplit. */
	TArray<int32> TrainIndices;
	TArray<int32> ValIndices;

	/** Split interval the index lists were built with (<= 0: no val split). */
	int32 EvalInterval = 8;

	/** Factor the cameras were divided by (images_2 -> 2). Informational (written to the _train.json summary). */
	int32 Downscale = 1;

	/** VkSplat scene_scale = ComputeSceneScale(Cameras) over ALL cameras; kept current by LoadColmapDataset and
	    AddDatasetImage. The option "scene_scale" overrides it per run. */
	double SceneScale = 0.0;

	/** COLMAP sparse dir: the points3D source of Init "colmap", and the summary's "colmap" field. May be empty. */
	FString SparseDir;

	/** Folder the images were decoded from (summary's "images" field only). May be empty. */
	FString ImagesDir;
	/** Phase 7: the images' alpha carries a per-pixel loss mask (0 = the pixel is ignored by the loss and the PSNR, 255 =
	    full weight; fractional at mask edges). False: alpha is 255 everywhere. */
	bool bHasMasks = false;
	/** Folder the masks were read from (informational). */
	FString MasksDir;
	/** Phase 8: per sorted camera, W*H fp16 bit patterns of the capture's depth in metres (0 = no supervision: sky, masked,
	    behind glass). Empty arrays when absent. */
	TArray<TArray<uint16>> Depths;
	bool bHasDepth = false;
	FString DepthDir;
	/** Phase 7b: per sorted camera, W*H RGBA8 background (the far scene / sky beyond the capture radius; render = C + T * B
	    in the loss). Empty arrays when absent (no background: C over black, as VkSplat). */
	TArray<TArray<uint32>> Backgrounds;
	bool bHasBackground = false;
	FString BackgroundDir;

	/** Rebuilds TrainIndices / ValIndices from EvalInterval and Cameras.Num(). */
	void RebuildSplit();
};

namespace HarmonyTraining
{
	/**
	 * TrainSplat's dataset loading, moved as-is (CONTRACT_P4 P0): downscale from the images folder suffix ("images_2" -> 2,
	 * "<x>_<n>_png" -> n and .png names), LoadColmapText (name-sorted), per-camera size check (11..4096 px per side),
	 * train/val split by EvalInterval (fails when no train image is left), parallel decode of the train images (and of
	 * the val images when bLoadValImages) to RGBA8 with alpha forced to 255, SceneScale = ComputeSceneScale(all cameras).
	 * Logs "TrainSplat: decoding ..." / "TrainSplat: decoded ..." as before. Game thread or any thread (ParallelFor inside).
	 * On failure OutError holds the message (no "ERROR: " prefix, nothing logged at Error level) and Out is reset.
	 */
	HARMONYTRAINING_API bool LoadColmapDataset(const FString& SparseDir, const FString& ImagesDir, int32 EvalInterval,
		FHarmonyTrainingDataset& Out, FString& OutError, bool bLoadValImages = true, const FString& MasksDir = FString(),
		const FString& DepthDir = FString(), const FString& BackgroundDir = FString());

	/**
	 * Inserts one camera + image into Dataset at its sorted position (case-sensitive byte order of ImageName), then
	 * rebuilds the split (Dataset.EvalInterval) and SceneScale. Pixels: Camera.Width*Camera.Height RGBA8, R in the low
	 * byte; alpha is forced to 255 here. Returns false (and logs a warning, Dataset unchanged) when the size is outside
	 * 11..4096 px per side, Pixels has the wrong count, or the ImageName is already present. Set Dataset.Downscale /
	 * SparseDir / EvalInterval yourself. Cost: O(N) per call (sorted insert + scene scale).
	 */
	HARMONYTRAINING_API bool AddDatasetImage(FHarmonyTrainingDataset& Dataset, const FHarmonyTrainingCamera& Camera, TArray<uint32>&& Pixels);

	/**
	 * UHarmonyTrainingLibrary::TrainSplat on an already loaded dataset: same options (OptionsJson keys of TrainSplat plus
	 * "out_asset"), same blocking chunk loop, same outputs (<OutPly>, <OutPly base>_train.json / _loss.f32, optional
	 * state dir / eval renders / asset) and the same "OK: ..." / "ERROR: ..." result line. Init: state dir, PLY or
	 * "colmap" (points3D from Dataset.SparseDir). The dataset's own train/val split is used; a different
	 * "eval_interval" option is ignored with a warning. Every image of Dataset.TrainIndices (and of ValIndices when eval
	 * is on) must be loaded. Game thread, blocking; Dataset is only referenced until the call returns.
	 */
	HARMONYTRAINING_API FString TrainSplatFromDataset(const FHarmonyTrainingDataset& Dataset, const FString& Init, int32 Steps,
		const FString& OutPly, const FString& OptionsJson);
}
