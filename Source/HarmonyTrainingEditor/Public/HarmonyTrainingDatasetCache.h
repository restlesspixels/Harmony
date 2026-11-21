// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FHarmonyTrainingDataset;

/**
 * HarmonyTraining Phase 6: the on-disk dataset cache of a capture (notes/phase6.md).
 *
 * Layout of one cache folder (exactly what LoadColmapDataset / TrainSplat read, plus a manifest):
 *   <dir>/harmony_dataset.json     manifest (FHarmonyDatasetCacheManifest; its presence marks a cache folder)
 *   <dir>/sparse/0/cameras.txt     COLMAP text model of the capture (Rotor's layout, sampler points with POINTS2D)
 *   <dir>/sparse/0/images.txt
 *   <dir>/sparse/0/points3D.txt    the sampler's init points (Init "colmap" rebuilds the session's exact init)
 *   <dir>/images_<ds>/frame_*.png  the training images (8-bit, the downscaled pixels the capture trained on)
 *   <dir>/mrq_capture.json         the capture summary (as before)
 * Later phases add channels next to images_<ds> (masks, normals, depth) and list them in the manifest.
 */
namespace HarmonyTrainingEditor
{
	constexpr int32 DatasetCacheVersion = 1;
	extern HARMONYTRAININGEDITOR_API const TCHAR* const DatasetCacheManifestName;   // "harmony_dataset.json"

	struct HARMONYTRAININGEDITOR_API FHarmonyDatasetCacheManifest
	{
		int32 Version = DatasetCacheVersion;
		FString Name;               // folder name
		FString Created;            // ISO 8601, local time
		FString Map;                // capture world (PIE prefix removed)
		FString Job;                // MRQ job name
		FString OutAsset;           // the capture's OutAsset (long package name; "" when none, or written before it was recorded)
		int32 NumImages = 0;
		int32 Downscale = 2;
		int32 EvalInterval = 8;
		int32 Width = 0;            // dataset (downscaled) size of the first image
		int32 Height = 0;
		double SceneScale = 0.0;
		FString InitMode;           // sampler / depth_seed / none
		int32 NumPoints = 0;        // points3D rows (sampler / depth seeds)
		FString SparseRel = TEXT("sparse/0");
		FString ImagesRel;          // images_<ds> (images for downscale 1)
		TMap<FString, FString> Channels;   // name -> relative folder ("mask" -> "masks_2", Phase 7)
		FString TrainOptionsJson;          // Phase 7: options the capture implies (cull_radius, cull_boxes); merged under the request's

		FString ToJson() const;
		static bool FromJson(const FString& Json, FHarmonyDatasetCacheManifest& Out, FString& OutError);
	};

	/** Absolute folder for a new cache: <project setting root>/<Prefix>_<yyyymmdd_hhmmss> (Prefix sanitised; unique). */
	HARMONYTRAININGEDITOR_API FString MakeDatasetCacheDir(const FString& Prefix);

	/** Writes <Dir>/harmony_dataset.json. */
	HARMONYTRAININGEDITOR_API bool WriteDatasetCacheManifest(const FString& Dir, const FHarmonyDatasetCacheManifest& Manifest, FString& OutError);

	/** Reads and checks a cache folder (manifest, sparse dir with cameras/images/points3D text, images folder). Dir may also
	    be a cache name under the project setting root. OutDir = the absolute folder. */
	HARMONYTRAININGEDITOR_API bool ReadDatasetCache(const FString& DirOrName, FHarmonyDatasetCacheManifest& Out, FString& OutDir, FString& OutError);

	/** Cache folders under the project setting root (those holding a manifest), newest first (manifest file time). */
	HARMONYTRAININGEDITOR_API TArray<FString> ListDatasetCaches();

	/** Deletes the oldest cache folders beyond MaxCachedDatasets (project setting; 0 = keep all), never Keep. Only folders
	    directly under the root that hold a manifest are counted. Also deletes (whatever MaxCachedDatasets says) the folders
	    of failed captures left without a manifest: directly under the root, no manifest, an mrq_capture.json reporting
	    "success": false (a failed capture whose dataset was still published keeps its folder until then). Never a pinned
	    folder. Returns the deleted folders. */
	HARMONYTRAININGEDITOR_API TArray<FString> PruneDatasetCaches(const FString& Keep);

	/** Pinned cache folders are never pruned (and do not count towards MaxCachedDatasets) until unpinned: a capture job
	    that needs several captures at once (base + variants) pins them while it runs. */
	HARMONYTRAININGEDITOR_API void PinDatasetCache(const FString& Dir, bool bPinned);

	/** Keys of Over on top of Under (both JSON objects; an empty side counts as {}). Invalid Over is returned as-is so the
	    trainer reports it. */
	HARMONYTRAININGEDITOR_API FString MergeJsonObjects(const FString& Under, const FString& Over);

	/** Phase 7: the options a cached dataset implies (its train_options, masks_dir, eval_interval) under RequestOptions. */
	HARMONYTRAININGEDITOR_API FString MakeCacheTrainingOptions(const FHarmonyDatasetCacheManifest& Manifest, const FString& Dir, const FString& RequestOptions);

	/** Exact comparison of two datasets (cameras field by field, train / val split, and every image both have loaded, byte
	    for byte). Returns "" when equal, else the first difference. */
	HARMONYTRAININGEDITOR_API FString CompareDatasets(const FHarmonyTrainingDataset& A, const FHarmonyTrainingDataset& B);
}
