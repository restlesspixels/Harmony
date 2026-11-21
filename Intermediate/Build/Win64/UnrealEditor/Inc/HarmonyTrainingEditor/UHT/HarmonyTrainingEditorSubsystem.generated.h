// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "HarmonyTrainingEditorSubsystem.h"

#ifdef HARMONYTRAININGEDITOR_HarmonyTrainingEditorSubsystem_generated_h
#error "HarmonyTrainingEditorSubsystem.generated.h already included, missing '#pragma once' in HarmonyTrainingEditorSubsystem.h"
#endif
#define HARMONYTRAININGEDITOR_HarmonyTrainingEditorSubsystem_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
struct FHarmonyTrainingRequest;
struct FHarmonyTrainingStatus;

// ********** Begin ScriptStruct FHarmonyTrainingRequest *******************************************
struct Z_Construct_UScriptStruct_FHarmonyTrainingRequest_Statics;
HARMONYTRAININGEDITOR_API UScriptStruct* Z_Construct_UScriptStruct_FHarmonyTrainingRequest(ETypeConstructPhase);

#define FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingEditorSubsystem_h_24_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FHarmonyTrainingRequest_Statics; \
	UE_NODEBUG static UScriptStruct* StaticStruct() { return Z_Construct_UScriptStruct_FHarmonyTrainingRequest(ETypeConstructPhase::Inner); }


struct FHarmonyTrainingRequest;
// ********** End ScriptStruct FHarmonyTrainingRequest *********************************************

// ********** Begin ScriptStruct FHarmonyTrainingStatus ********************************************
struct Z_Construct_UScriptStruct_FHarmonyTrainingStatus_Statics;
HARMONYTRAININGEDITOR_API UScriptStruct* Z_Construct_UScriptStruct_FHarmonyTrainingStatus(ETypeConstructPhase);

#define FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingEditorSubsystem_h_88_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FHarmonyTrainingStatus_Statics; \
	UE_NODEBUG static UScriptStruct* StaticStruct() { return Z_Construct_UScriptStruct_FHarmonyTrainingStatus(ETypeConstructPhase::Inner); }


struct FHarmonyTrainingStatus;
// ********** End ScriptStruct FHarmonyTrainingStatus **********************************************

// ********** Begin Class UHarmonyTrainingEditorSubsystem ******************************************
#define FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingEditorSubsystem_h_150_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execGetValCameraView); \
	DECLARE_FUNCTION(execSaveAsset); \
	DECLARE_FUNCTION(execSetLivePreview); \
	DECLARE_FUNCTION(execResetFrameStats); \
	DECLARE_FUNCTION(execGetStatus); \
	DECLARE_FUNCTION(execIsSavingAsset); \
	DECLARE_FUNCTION(execIsTraining); \
	DECLARE_FUNCTION(execStopAndFinalize); \
	DECLARE_FUNCTION(execCancelTraining); \
	DECLARE_FUNCTION(execResumeTraining); \
	DECLARE_FUNCTION(execPauseTraining); \
	DECLARE_FUNCTION(execListDatasetCaches); \
	DECLARE_FUNCTION(execStartTrainingFromCache); \
	DECLARE_FUNCTION(execStartTraining);


struct Z_Construct_UClass_UHarmonyTrainingEditorSubsystem_Statics;
HARMONYTRAININGEDITOR_API UClass* Z_Construct_UClass_UHarmonyTrainingEditorSubsystem(ETypeConstructPhase);

#define FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingEditorSubsystem_h_150_INCLASS_NO_PURE_DECLS \
private: \
	friend struct ::Z_Construct_UClass_UHarmonyTrainingEditorSubsystem_Statics; \
	friend HARMONYTRAININGEDITOR_API UClass* ::Z_Construct_UClass_UHarmonyTrainingEditorSubsystem(ETypeConstructPhase); \
public: \
	DECLARE_CLASS2(UHarmonyTrainingEditorSubsystem, UEditorSubsystem, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/HarmonyTrainingEditor"), Z_Construct_UClass_UHarmonyTrainingEditorSubsystem) \
	DECLARE_SERIALIZER(UHarmonyTrainingEditorSubsystem)


#define FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingEditorSubsystem_h_150_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UHarmonyTrainingEditorSubsystem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	/** Deleted move- and copy-constructors, should never be used */ \
	UHarmonyTrainingEditorSubsystem(UHarmonyTrainingEditorSubsystem&&) = delete; \
	UHarmonyTrainingEditorSubsystem(const UHarmonyTrainingEditorSubsystem&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UHarmonyTrainingEditorSubsystem); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UHarmonyTrainingEditorSubsystem); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UHarmonyTrainingEditorSubsystem) \
	NO_API virtual ~UHarmonyTrainingEditorSubsystem();


#define FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingEditorSubsystem_h_147_PROLOG
#define FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingEditorSubsystem_h_150_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingEditorSubsystem_h_150_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingEditorSubsystem_h_150_INCLASS_NO_PURE_DECLS \
	FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingEditorSubsystem_h_150_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UHarmonyTrainingEditorSubsystem;

// ********** End Class UHarmonyTrainingEditorSubsystem ********************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingEditorSubsystem_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
