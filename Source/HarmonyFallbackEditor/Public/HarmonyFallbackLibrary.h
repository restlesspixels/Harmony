// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HarmonyFallbackSettings.h"
#include "HarmonyFallbackLibrary.generated.h"

class AActor;
class UHarmonyDataAsset;

/**
 * Entry points of the native fallback-mesh builder (Docs/Dev/SplatToMesh_M3_NativePort.md).
 * Python: unreal.HarmonyFallbackLibrary.build_fallback_from_cache(cache, settings, "/Env_X/FallbackMesh/Native")
 */
UCLASS()
class HARMONYFALLBACKEDITOR_API UHarmonyFallbackLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Builds the fallback meshes of every tier in Settings from a dataset cache (a folder, or a cache name under the
	 * project dataset cache root) and writes the assets under OutputPackagePath (a long package path such as
	 * "/Env_Showroom/FallbackMesh/Native"; empty = "<mount of the OutAsset recorded in the cache>/FallbackMesh").
	 * Blocking, game thread. Returns "OK: ..." (a JSON summary follows the prefix) or "ERROR: ...".
	 */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Fallback")
	static FString BuildFallbackFromCache(const FString& Cache, const FHarmonyFallbackSettings& Settings, const FString& OutputPackagePath);

	/**
	 * Builds variant options on top of a base fallback (Docs/Dev/SplatToMesh_M4_Variants.md). BaseCache is the capture the
	 * base was built from, BaseOutputPackagePath the folder BuildFallbackFromCache wrote it to (it must hold
	 * SM_<name>_Fallback_<tier> of every tier in Settings). Every option's capture must have the base's poses. Per option
	 * the depth is compared with the base's: where nothing moved, the base meshes and UVs are kept and only the atlas
	 * pages (with the sky dome), the floor's cubemap and the material instances are baked from the option's capture;
	 * else the option is built as a complete fallback of its own. Assets go to <BaseOutputPackagePath>/<Set>_<Option>.
	 * With Settings.IntermediateDir, <dir>/<Set>_<Option>/detect.json (and the changed voxels) are written.
	 * Blocking, game thread. Returns "OK: ..." (a JSON summary follows the prefix) or "ERROR: ...".
	 */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Fallback")
	static FString BuildFallbackOptions(const FString& BaseCache, const TArray<FHarmonyFallbackOptionSource>& Options,
		const FHarmonyFallbackSettings& Settings, const FString& BaseOutputPackagePath);

	/**
	 * Only the change detection of BuildFallbackOptions: compares the depth (and images) of two captures with the same
	 * poses and returns the report as JSON ("ERROR: ..." on failure); OutputDir, when set, receives detect.json and
	 * detect_voxels.npy. Uses the Variant* fields of Settings.
	 */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Fallback")
	static FString DetectFallbackGeometryChange(const FString& BaseCache, const FString& OptionCache, const FHarmonyFallbackSettings& Settings,
		const FString& OutputDir);

	/**
	 * Editor review helper: shows one variant option on a placed fallback actor. The actor's static mesh component must
	 * show a mesh recorded in Asset->FallbackTiers (a tier's base mesh or one of its options' meshes). Option None (or the
	 * set's base option) restores the base. Not a runtime switch: it loads the option's assets synchronously and edits
	 * the component. Returns "OK: ..." or "ERROR: ...".
	 */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Fallback")
	static FString ApplyFallbackOption(AActor* Actor, UHarmonyDataAsset* Asset, FName VariantSet, FName Option);

	/** The default settings (the M1 recipe with the cubemap floor), for scripts that only change a few fields. */
	UFUNCTION(BlueprintPure, Category = "Harmony|Fallback")
	static FHarmonyFallbackSettings GetDefaultFallbackSettings();
};
