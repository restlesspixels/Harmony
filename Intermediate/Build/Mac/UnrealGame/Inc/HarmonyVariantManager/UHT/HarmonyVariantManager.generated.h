// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "HarmonyVariantManager.h"

#ifdef HARMONYVARIANTMANAGER_HarmonyVariantManager_generated_h
#error "HarmonyVariantManager.generated.h already included, missing '#pragma once' in HarmonyVariantManager.h"
#endif
#define HARMONYVARIANTMANAGER_HarmonyVariantManager_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin ScriptStruct FHarmonyLevelVariantSetsSaved *************************************
struct Z_Construct_UScriptStruct_FHarmonyLevelVariantSetsSaved_Statics;
HARMONYVARIANTMANAGER_API UScriptStruct* Z_Construct_UScriptStruct_FHarmonyLevelVariantSetsSaved(ETypeConstructPhase);

#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyVariantManager_Public_HarmonyVariantManager_h_21_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FHarmonyLevelVariantSetsSaved_Statics; \
	UE_NODEBUG static UScriptStruct* StaticStruct() { return Z_Construct_UScriptStruct_FHarmonyLevelVariantSetsSaved(ETypeConstructPhase::Inner); }


struct FHarmonyLevelVariantSetsSaved;
// ********** End ScriptStruct FHarmonyLevelVariantSetsSaved ***************************************

// ********** Begin Class UHarmonyLevelVariantSetsSource *******************************************
struct Z_Construct_UClass_UHarmonyLevelVariantSetsSource_Statics;
HARMONYVARIANTMANAGER_API UClass* Z_Construct_UClass_UHarmonyLevelVariantSetsSource(ETypeConstructPhase);

#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyVariantManager_Public_HarmonyVariantManager_h_42_INCLASS_NO_PURE_DECLS \
private: \
	friend struct ::Z_Construct_UClass_UHarmonyLevelVariantSetsSource_Statics; \
	friend HARMONYVARIANTMANAGER_API UClass* ::Z_Construct_UClass_UHarmonyLevelVariantSetsSource(ETypeConstructPhase); \
public: \
	DECLARE_CLASS2(UHarmonyLevelVariantSetsSource, UHarmonyVariantSource, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/HarmonyVariantManager"), Z_Construct_UClass_UHarmonyLevelVariantSetsSource) \
	DECLARE_SERIALIZER(UHarmonyLevelVariantSetsSource)


#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyVariantManager_Public_HarmonyVariantManager_h_42_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UHarmonyLevelVariantSetsSource(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	/** Deleted move- and copy-constructors, should never be used */ \
	UHarmonyLevelVariantSetsSource(UHarmonyLevelVariantSetsSource&&) = delete; \
	UHarmonyLevelVariantSetsSource(const UHarmonyLevelVariantSetsSource&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UHarmonyLevelVariantSetsSource); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UHarmonyLevelVariantSetsSource); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UHarmonyLevelVariantSetsSource) \
	NO_API virtual ~UHarmonyLevelVariantSetsSource();


#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyVariantManager_Public_HarmonyVariantManager_h_39_PROLOG
#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyVariantManager_Public_HarmonyVariantManager_h_42_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyVariantManager_Public_HarmonyVariantManager_h_42_INCLASS_NO_PURE_DECLS \
	FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyVariantManager_Public_HarmonyVariantManager_h_42_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UHarmonyLevelVariantSetsSource;

// ********** End Class UHarmonyLevelVariantSetsSource *********************************************

// ********** Begin Class UHarmonyVariantManagerSync ***********************************************
#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyVariantManager_Public_HarmonyVariantManager_h_78_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execSyncNow);


struct Z_Construct_UClass_UHarmonyVariantManagerSync_Statics;
HARMONYVARIANTMANAGER_API UClass* Z_Construct_UClass_UHarmonyVariantManagerSync(ETypeConstructPhase);

#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyVariantManager_Public_HarmonyVariantManager_h_78_INCLASS_NO_PURE_DECLS \
private: \
	friend struct ::Z_Construct_UClass_UHarmonyVariantManagerSync_Statics; \
	friend HARMONYVARIANTMANAGER_API UClass* ::Z_Construct_UClass_UHarmonyVariantManagerSync(ETypeConstructPhase); \
public: \
	DECLARE_CLASS2(UHarmonyVariantManagerSync, UActorComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/HarmonyVariantManager"), Z_Construct_UClass_UHarmonyVariantManagerSync) \
	DECLARE_SERIALIZER(UHarmonyVariantManagerSync)


#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyVariantManager_Public_HarmonyVariantManager_h_78_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	UHarmonyVariantManagerSync(UHarmonyVariantManagerSync&&) = delete; \
	UHarmonyVariantManagerSync(const UHarmonyVariantManagerSync&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UHarmonyVariantManagerSync); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UHarmonyVariantManagerSync); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UHarmonyVariantManagerSync) \
	NO_API virtual ~UHarmonyVariantManagerSync();


#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyVariantManager_Public_HarmonyVariantManager_h_75_PROLOG
#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyVariantManager_Public_HarmonyVariantManager_h_78_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyVariantManager_Public_HarmonyVariantManager_h_78_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyVariantManager_Public_HarmonyVariantManager_h_78_INCLASS_NO_PURE_DECLS \
	FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyVariantManager_Public_HarmonyVariantManager_h_78_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UHarmonyVariantManagerSync;

// ********** End Class UHarmonyVariantManagerSync *************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyVariantManager_Public_HarmonyVariantManager_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
