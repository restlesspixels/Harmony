// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "HarmonyTrainingMoviePipelineOutput.h"

#ifdef HARMONYTRAININGEDITOR_HarmonyTrainingMoviePipelineOutput_generated_h
#error "HarmonyTrainingMoviePipelineOutput.generated.h already included, missing '#pragma once' in HarmonyTrainingMoviePipelineOutput.h"
#endif
#define HARMONYTRAININGEDITOR_HarmonyTrainingMoviePipelineOutput_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "Templates/IsUEnumClass.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
struct FHarmonyTrainingCaptureResult;
struct FHarmonyTrainingRequest;

// ********** Begin ScriptStruct FHarmonyTrainingCaptureResult *************************************
struct Z_Construct_UScriptStruct_FHarmonyTrainingCaptureResult_Statics;
HARMONYTRAININGEDITOR_API UScriptStruct* Z_Construct_UScriptStruct_FHarmonyTrainingCaptureResult(ETypeConstructPhase);

#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingMoviePipelineOutput_h_51_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FHarmonyTrainingCaptureResult_Statics; \
	UE_NODEBUG static UScriptStruct* StaticStruct() { return Z_Construct_UScriptStruct_FHarmonyTrainingCaptureResult(ETypeConstructPhase::Inner); }


struct FHarmonyTrainingCaptureResult;
// ********** End ScriptStruct FHarmonyTrainingCaptureResult ***************************************

// ********** Begin Class UHarmonyTrainingMoviePipelineOutput **************************************
#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingMoviePipelineOutput_h_166_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execReleaseLastCapture); \
	DECLARE_FUNCTION(execVerifyLastCaptureCache); \
	DECLARE_FUNCTION(execTrainLastCaptureBlocking); \
	DECLARE_FUNCTION(execStartTrainingFromLastCapture); \
	DECLARE_FUNCTION(execHasLastCapture); \
	DECLARE_FUNCTION(execGetLastCaptureCount); \
	DECLARE_FUNCTION(execGetLastCaptureResult);


struct Z_Construct_UClass_UHarmonyTrainingMoviePipelineOutput_Statics;
HARMONYTRAININGEDITOR_API UClass* Z_Construct_UClass_UHarmonyTrainingMoviePipelineOutput(ETypeConstructPhase);

#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingMoviePipelineOutput_h_166_INCLASS_NO_PURE_DECLS \
private: \
	friend struct ::Z_Construct_UClass_UHarmonyTrainingMoviePipelineOutput_Statics; \
	friend HARMONYTRAININGEDITOR_API UClass* ::Z_Construct_UClass_UHarmonyTrainingMoviePipelineOutput(ETypeConstructPhase); \
public: \
	DECLARE_CLASS2(UHarmonyTrainingMoviePipelineOutput, UMoviePipelineOutputBase, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/HarmonyTrainingEditor"), Z_Construct_UClass_UHarmonyTrainingMoviePipelineOutput) \
	DECLARE_SERIALIZER(UHarmonyTrainingMoviePipelineOutput)


#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingMoviePipelineOutput_h_166_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	UHarmonyTrainingMoviePipelineOutput(UHarmonyTrainingMoviePipelineOutput&&) = delete; \
	UHarmonyTrainingMoviePipelineOutput(const UHarmonyTrainingMoviePipelineOutput&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UHarmonyTrainingMoviePipelineOutput); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UHarmonyTrainingMoviePipelineOutput); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UHarmonyTrainingMoviePipelineOutput) \
	NO_API virtual ~UHarmonyTrainingMoviePipelineOutput();


#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingMoviePipelineOutput_h_163_PROLOG
#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingMoviePipelineOutput_h_166_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingMoviePipelineOutput_h_166_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingMoviePipelineOutput_h_166_INCLASS_NO_PURE_DECLS \
	FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingMoviePipelineOutput_h_166_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UHarmonyTrainingMoviePipelineOutput;

// ********** End Class UHarmonyTrainingMoviePipelineOutput ****************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingMoviePipelineOutput_h

// ********** Begin Enum EHarmonyTrainingInitMode **************************************************
#define FOREACH_ENUM_EHARMONYTRAININGINITMODE(op) \
	op(EHarmonyTrainingInitMode::Sampler) \
	op(EHarmonyTrainingInitMode::DepthSeed) \
	op(EHarmonyTrainingInitMode::None) 

enum class EHarmonyTrainingInitMode : uint8;
template<> struct TIsUEnumClass<EHarmonyTrainingInitMode> { enum { Value = true }; };
template<> UE_NODEBUG HARMONYTRAININGEDITOR_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyTrainingInitMode>();
// ********** End Enum EHarmonyTrainingInitMode ****************************************************

// ********** Begin Enum EHarmonyTrainingCacheMode *************************************************
#define FOREACH_ENUM_EHARMONYTRAININGCACHEMODE(op) \
	op(EHarmonyTrainingCacheMode::ProjectDefault) \
	op(EHarmonyTrainingCacheMode::Always) \
	op(EHarmonyTrainingCacheMode::Never) 

enum class EHarmonyTrainingCacheMode : uint8;
template<> struct TIsUEnumClass<EHarmonyTrainingCacheMode> { enum { Value = true }; };
template<> UE_NODEBUG HARMONYTRAININGEDITOR_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyTrainingCacheMode>();
// ********** End Enum EHarmonyTrainingCacheMode ***************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
