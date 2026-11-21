// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "HarmonyFallbackLibrary.h"

#ifdef HARMONYFALLBACKEDITOR_HarmonyFallbackLibrary_generated_h
#error "HarmonyFallbackLibrary.generated.h already included, missing '#pragma once' in HarmonyFallbackLibrary.h"
#endif
#define HARMONYFALLBACKEDITOR_HarmonyFallbackLibrary_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class AActor;
class UHarmonyDataAsset;
struct FHarmonyFallbackOptionSource;
struct FHarmonyFallbackSettings;

// ********** Begin Class UHarmonyFallbackLibrary **************************************************
#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackLibrary_h_20_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execGetDefaultFallbackSettings); \
	DECLARE_FUNCTION(execApplyFallbackOption); \
	DECLARE_FUNCTION(execDetectFallbackGeometryChange); \
	DECLARE_FUNCTION(execBuildFallbackOptions); \
	DECLARE_FUNCTION(execBuildFallbackFromCache);


struct Z_Construct_UClass_UHarmonyFallbackLibrary_Statics;
HARMONYFALLBACKEDITOR_API UClass* Z_Construct_UClass_UHarmonyFallbackLibrary(ETypeConstructPhase);

#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackLibrary_h_20_INCLASS_NO_PURE_DECLS \
private: \
	friend struct ::Z_Construct_UClass_UHarmonyFallbackLibrary_Statics; \
	friend HARMONYFALLBACKEDITOR_API UClass* ::Z_Construct_UClass_UHarmonyFallbackLibrary(ETypeConstructPhase); \
public: \
	DECLARE_CLASS2(UHarmonyFallbackLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/HarmonyFallbackEditor"), Z_Construct_UClass_UHarmonyFallbackLibrary) \
	DECLARE_SERIALIZER(UHarmonyFallbackLibrary)


#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackLibrary_h_20_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UHarmonyFallbackLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	/** Deleted move- and copy-constructors, should never be used */ \
	UHarmonyFallbackLibrary(UHarmonyFallbackLibrary&&) = delete; \
	UHarmonyFallbackLibrary(const UHarmonyFallbackLibrary&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UHarmonyFallbackLibrary); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UHarmonyFallbackLibrary); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UHarmonyFallbackLibrary) \
	NO_API virtual ~UHarmonyFallbackLibrary();


#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackLibrary_h_17_PROLOG
#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackLibrary_h_20_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackLibrary_h_20_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackLibrary_h_20_INCLASS_NO_PURE_DECLS \
	FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackLibrary_h_20_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UHarmonyFallbackLibrary;

// ********** End Class UHarmonyFallbackLibrary ****************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackLibrary_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
