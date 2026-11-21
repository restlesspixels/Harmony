// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "HarmonyFallbackSettings.h"

#ifdef HARMONYFALLBACKEDITOR_HarmonyFallbackSettings_generated_h
#error "HarmonyFallbackSettings.generated.h already included, missing '#pragma once' in HarmonyFallbackSettings.h"
#endif
#define HARMONYFALLBACKEDITOR_HarmonyFallbackSettings_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "Templates/IsUEnumClass.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin ScriptStruct FHarmonyFallbackTier **********************************************
struct Z_Construct_UScriptStruct_FHarmonyFallbackTier_Statics;
HARMONYFALLBACKEDITOR_API UScriptStruct* Z_Construct_UScriptStruct_FHarmonyFallbackTier(ETypeConstructPhase);

#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackSettings_h_12_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FHarmonyFallbackTier_Statics; \
	UE_NODEBUG static UScriptStruct* StaticStruct() { return Z_Construct_UScriptStruct_FHarmonyFallbackTier(ETypeConstructPhase::Inner); }


struct FHarmonyFallbackTier;
// ********** End ScriptStruct FHarmonyFallbackTier ************************************************

// ********** Begin ScriptStruct FHarmonyFallbackOptionSource **************************************
struct Z_Construct_UScriptStruct_FHarmonyFallbackOptionSource_Statics;
HARMONYFALLBACKEDITOR_API UScriptStruct* Z_Construct_UScriptStruct_FHarmonyFallbackOptionSource(ETypeConstructPhase);

#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackSettings_h_47_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FHarmonyFallbackOptionSource_Statics; \
	UE_NODEBUG static UScriptStruct* StaticStruct() { return Z_Construct_UScriptStruct_FHarmonyFallbackOptionSource(ETypeConstructPhase::Inner); }


struct FHarmonyFallbackOptionSource;
// ********** End ScriptStruct FHarmonyFallbackOptionSource ****************************************

// ********** Begin ScriptStruct FHarmonyFallbackSettings ******************************************
struct Z_Construct_UScriptStruct_FHarmonyFallbackSettings_Statics;
HARMONYFALLBACKEDITOR_API UScriptStruct* Z_Construct_UScriptStruct_FHarmonyFallbackSettings(ETypeConstructPhase);

#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackSettings_h_68_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FHarmonyFallbackSettings_Statics; \
	UE_NODEBUG static UScriptStruct* StaticStruct() { return Z_Construct_UScriptStruct_FHarmonyFallbackSettings(ETypeConstructPhase::Inner); }


struct FHarmonyFallbackSettings;
// ********** End ScriptStruct FHarmonyFallbackSettings ********************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackSettings_h

// ********** Begin Enum EHarmonyFallbackVariantGeometry *******************************************
#define FOREACH_ENUM_EHARMONYFALLBACKVARIANTGEOMETRY(op) \
	op(EHarmonyFallbackVariantGeometry::Auto) \
	op(EHarmonyFallbackVariantGeometry::ShareBaseGeometry) \
	op(EHarmonyFallbackVariantGeometry::OwnMesh) 

enum class EHarmonyFallbackVariantGeometry : uint8;
template<> struct TIsUEnumClass<EHarmonyFallbackVariantGeometry> { enum { Value = true }; };
template<> UE_NODEBUG HARMONYFALLBACKEDITOR_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyFallbackVariantGeometry>();
// ********** End Enum EHarmonyFallbackVariantGeometry *********************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
