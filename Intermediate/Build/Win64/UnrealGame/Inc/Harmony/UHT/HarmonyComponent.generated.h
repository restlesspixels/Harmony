// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "HarmonyComponent.h"

#ifdef HARMONY_HarmonyComponent_generated_h
#error "HarmonyComponent.generated.h already included, missing '#pragma once' in HarmonyComponent.h"
#endif
#define HARMONY_HarmonyComponent_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "Templates/IsUEnumClass.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin ScriptStruct FHarmonyAlphaFadeSettings *****************************************
struct Z_Construct_UScriptStruct_FHarmonyAlphaFadeSettings_Statics;
HARMONY_API UScriptStruct* Z_Construct_UScriptStruct_FHarmonyAlphaFadeSettings(ETypeConstructPhase);

#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h_21_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FHarmonyAlphaFadeSettings_Statics; \
	UE_NODEBUG static UScriptStruct* StaticStruct() { return Z_Construct_UScriptStruct_FHarmonyAlphaFadeSettings(ETypeConstructPhase::Inner); }


struct FHarmonyAlphaFadeSettings;
// ********** End ScriptStruct FHarmonyAlphaFadeSettings *******************************************

// ********** Begin ScriptStruct FHarmonyDepthOffsetSettings ***************************************
struct Z_Construct_UScriptStruct_FHarmonyDepthOffsetSettings_Statics;
HARMONY_API UScriptStruct* Z_Construct_UScriptStruct_FHarmonyDepthOffsetSettings(ETypeConstructPhase);

#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h_39_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FHarmonyDepthOffsetSettings_Statics; \
	UE_NODEBUG static UScriptStruct* StaticStruct() { return Z_Construct_UScriptStruct_FHarmonyDepthOffsetSettings(ETypeConstructPhase::Inner); }


struct FHarmonyDepthOffsetSettings;
// ********** End ScriptStruct FHarmonyDepthOffsetSettings *****************************************

// ********** Begin Class UHarmonyComponent ********************************************************
#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h_63_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execGetVariantOptionNames); \
	DECLARE_FUNCTION(execGetVariantSetNames); \
	DECLARE_FUNCTION(execClearVariants); \
	DECLARE_FUNCTION(execIsVariantTransitionActive); \
	DECLARE_FUNCTION(execSetVariantWithDuration); \
	DECLARE_FUNCTION(execSetVariant);


struct Z_Construct_UClass_UHarmonyComponent_Statics;
HARMONY_API UClass* Z_Construct_UClass_UHarmonyComponent(ETypeConstructPhase);

#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h_63_INCLASS_NO_PURE_DECLS \
private: \
	friend struct ::Z_Construct_UClass_UHarmonyComponent_Statics; \
	friend HARMONY_API UClass* ::Z_Construct_UClass_UHarmonyComponent(ETypeConstructPhase); \
public: \
	DECLARE_CLASS2(UHarmonyComponent, USceneComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/Harmony"), Z_Construct_UClass_UHarmonyComponent) \
	DECLARE_SERIALIZER(UHarmonyComponent)


#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h_63_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	UHarmonyComponent(UHarmonyComponent&&) = delete; \
	UHarmonyComponent(const UHarmonyComponent&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UHarmonyComponent); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UHarmonyComponent); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UHarmonyComponent) \
	NO_API virtual ~UHarmonyComponent();


#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h_60_PROLOG
#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h_63_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h_63_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h_63_INCLASS_NO_PURE_DECLS \
	FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h_63_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UHarmonyComponent;

// ********** End Class UHarmonyComponent **********************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h

// ********** Begin Enum EHarmonySplatDepthOffsetMode **********************************************
#define FOREACH_ENUM_EHARMONYSPLATDEPTHOFFSETMODE(op) \
	op(EHarmonySplatDepthOffsetMode::SimpleViewSpace) \
	op(EHarmonySplatDepthOffsetMode::GroundWeighted) 

enum class EHarmonySplatDepthOffsetMode : uint8;
template<> struct TIsUEnumClass<EHarmonySplatDepthOffsetMode> { enum { Value = true }; };
template<> UE_NODEBUG HARMONY_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonySplatDepthOffsetMode>();
// ********** End Enum EHarmonySplatDepthOffsetMode ************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
