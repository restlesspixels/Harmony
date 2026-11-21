// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "HarmonyTrainingSettings.generated.h"

/**
 * Project-wide defaults of HarmonyTraining (Phase 6; Project Settings > Plugins > Harmony Training, saved to
 * Config/DefaultGame.ini). Per-capture settings (the MRQ output, the capture rig) override them; a training request's
 * own Steps / OptionsJson keys override them too.
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Harmony Training"))
class HARMONYTRAININGEDITOR_API UHarmonyTrainingSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UHarmonyTrainingSettings();

	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }

	// ---- Training ----

	/** Steps of a training request whose Steps is 0 (MRQ output TrainingSteps 0, subsystem requests with Steps 0). */
	UPROPERTY(config, EditAnywhere, Category = "Training", meta = (ClampMin = 100))
	int32 DefaultTrainingSteps = 100000;

	/** TrainSplat options every in-editor request starts from; the request's own keys win (merged per key). */
	UPROPERTY(config, EditAnywhere, Category = "Training")
	FString DefaultTrainingOptionsJson = TEXT("{\"strategy\":\"mcmc\"}");

	// ---- Dataset cache ----

	/** Captures write their dataset (COLMAP text model with the sampler's points, downscaled PNGs, harmony_dataset.json)
	    into <DatasetCacheRoot>/<capture name>, so they can be trained again without rendering (TrainFromCache,
	    StartTrainingFromCache). A capture's own setting can override this; the capture rig's buttons always cache
	    (Retrain From Last Capture and Bake Variants need it). */
	UPROPERTY(config, EditAnywhere, Category = "Dataset Cache")
	bool bCacheCaptureDatasets = true;

	/** Root folder of the dataset cache. Relative paths are relative to the project folder. Empty: Saved/HarmonyTraining/Datasets. */
	UPROPERTY(config, EditAnywhere, Category = "Dataset Cache")
	FString DatasetCacheRoot = TEXT("Saved/HarmonyTraining/Datasets");

	/** Cached datasets kept: after a capture writes one, the oldest cache folders beyond this count are deleted (only folders
	    under DatasetCacheRoot that hold a harmony_dataset.json; pinned caches of a running bake are never deleted). 0 = keep
	    all. Folders of failed captures (no manifest, mrq_capture.json success false) are always removed. */
	UPROPERTY(config, EditAnywhere, Category = "Dataset Cache", meta = (ClampMin = 0))
	int32 MaxCachedDatasets = 3;

	// ---- Capture (Phase 7) ----

	/** A capture rig whose CaptureRadiusMode is ProjectDefault trains everything the cameras see when true; when false,
	    only what lies within DefaultCaptureRadiusMeters of the world origin (farther pixels and the sky are masked out of
	    the loss and UE's own sky shows behind the splats at runtime).
	    Default true for now (notes/phase7.md): masking the sky also masks the glass in front of it, so glass seen against
	    the sky goes unsupervised until training composites a background image (roadmap P7b). */
	UPROPERTY(config, EditAnywhere, Category = "Capture")
	bool bTrainToInfinity = true;

	UPROPERTY(config, EditAnywhere, Category = "Capture", meta = (ClampMin = 1.0, EditCondition = "!bTrainToInfinity"))
	float DefaultCaptureRadiusMeters = 2000.0f;

	/** Absolute path of DatasetCacheRoot. */
	FString GetDatasetCacheRootAbsolute() const;

	/** DefaultTrainingOptionsJson with Override's keys on top (both JSON objects; an empty / invalid side is skipped). */
	FString MergeTrainingOptions(const FString& Override) const;
};
