// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "HarmonyTrainingEditorDepthLibrary.generated.h"

/**
 * Python / Blueprint entry of the Phase 4 P2 depth-seeded init (CONTRACT_P4.md section 3), used headless by
 * Scripts/EnvGen/splat_depth_init.py (SplatDepthInit.ps1, UnrealEditor-Cmd -run=pythonscript -AllowCommandletRendering).
 * Python: unreal.HarmonyTrainingEditorDepthLibrary.build_depth_seed_init(map_path, sparse_dir, images_dir, out_state_dir, options_json)
 */
UCLASS()
class HARMONYTRAININGEDITOR_API UHarmonyTrainingEditorDepthLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Loads MapPath (a long package name such as /Env_Showroom/Dev/SplatCapture/Env_Showroom_SplatCapture; empty: the
	 * current editor world), replays every seeded COLMAP pose of SparseDir with a SceneCapture2D depth capture, seeds with
	 * HarmonyTraining::FDepthSeedAccumulator (colours from ImagesDir, e.g. images_2) and writes the init state dir
	 * OutStateDir (TrainSplat's Init), <OutStateDir>/seeds/ (COLMAP model with the seeds as points3D.txt) and
	 * <OutStateDir>/depth_init.json. OptionsJson: see HarmonyTraining::ParseDepthSeedOptions. Returns "OK: ..." or
	 * "ERROR: ...". Game thread; blocks for the whole capture.
	 */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	static FString BuildDepthSeedInit(const FString& MapPath, const FString& SparseDir, const FString& ImagesDir,
		const FString& OutStateDir, const FString& OptionsJson);
};
