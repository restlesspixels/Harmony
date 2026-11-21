// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "HarmonyCaptureJobSubsystem.h"

#ifdef HARMONYTRAININGEDITOR_HarmonyCaptureJobSubsystem_generated_h
#error "HarmonyCaptureJobSubsystem.generated.h already included, missing '#pragma once' in HarmonyCaptureJobSubsystem.h"
#endif
#define HARMONYTRAININGEDITOR_HarmonyCaptureJobSubsystem_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class AHarmonyCaptureRig;

// ********** Begin Class UHarmonyCaptureJobSubsystem **********************************************
#define FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyCaptureJobSubsystem_h_37_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execOnTrainingFinished); \
	DECLARE_FUNCTION(execGetLastResult); \
	DECLARE_FUNCTION(execGetStatus); \
	DECLARE_FUNCTION(execIsBusy); \
	DECLARE_FUNCTION(execCancel); \
	DECLARE_FUNCTION(execFindLastOptionCapture); \
	DECLARE_FUNCTION(execFindLastBaseCapture); \
	DECLARE_FUNCTION(execRetrainFromLastCapture); \
	DECLARE_FUNCTION(execBakeVariants); \
	DECLARE_FUNCTION(execCaptureAndTrain); \
	DECLARE_FUNCTION(execCaptureOnly);


struct Z_Construct_UClass_UHarmonyCaptureJobSubsystem_Statics;
HARMONYTRAININGEDITOR_API UClass* Z_Construct_UClass_UHarmonyCaptureJobSubsystem(ETypeConstructPhase);

#define FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyCaptureJobSubsystem_h_37_INCLASS_NO_PURE_DECLS \
private: \
	friend struct ::Z_Construct_UClass_UHarmonyCaptureJobSubsystem_Statics; \
	friend HARMONYTRAININGEDITOR_API UClass* ::Z_Construct_UClass_UHarmonyCaptureJobSubsystem(ETypeConstructPhase); \
public: \
	DECLARE_CLASS2(UHarmonyCaptureJobSubsystem, UEditorSubsystem, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/HarmonyTrainingEditor"), Z_Construct_UClass_UHarmonyCaptureJobSubsystem) \
	DECLARE_SERIALIZER(UHarmonyCaptureJobSubsystem)


#define FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyCaptureJobSubsystem_h_37_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UHarmonyCaptureJobSubsystem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	/** Deleted move- and copy-constructors, should never be used */ \
	UHarmonyCaptureJobSubsystem(UHarmonyCaptureJobSubsystem&&) = delete; \
	UHarmonyCaptureJobSubsystem(const UHarmonyCaptureJobSubsystem&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UHarmonyCaptureJobSubsystem); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UHarmonyCaptureJobSubsystem); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UHarmonyCaptureJobSubsystem) \
	NO_API virtual ~UHarmonyCaptureJobSubsystem();


#define FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyCaptureJobSubsystem_h_34_PROLOG
#define FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyCaptureJobSubsystem_h_37_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyCaptureJobSubsystem_h_37_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyCaptureJobSubsystem_h_37_INCLASS_NO_PURE_DECLS \
	FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyCaptureJobSubsystem_h_37_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UHarmonyCaptureJobSubsystem;

// ********** End Class UHarmonyCaptureJobSubsystem ************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyCaptureJobSubsystem_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
