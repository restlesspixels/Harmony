// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "HarmonyCaptureRig.h"

#ifdef HARMONYCAPTURE_HarmonyCaptureRig_generated_h
#error "HarmonyCaptureRig.generated.h already included, missing '#pragma once' in HarmonyCaptureRig.h"
#endif
#define HARMONYCAPTURE_HarmonyCaptureRig_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "Templates/IsUEnumClass.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class AActor;

// ********** Begin ScriptStruct FHarmonyVariantProbeOverride **************************************
struct Z_Construct_UScriptStruct_FHarmonyVariantProbeOverride_Statics;
HARMONYCAPTURE_API UScriptStruct* Z_Construct_UScriptStruct_FHarmonyVariantProbeOverride(ETypeConstructPhase);

#define FID_HostProject_Plugins_Harmony_Source_HarmonyCapture_Public_HarmonyCaptureRig_h_89_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FHarmonyVariantProbeOverride_Statics; \
	UE_NODEBUG static UScriptStruct* StaticStruct() { return Z_Construct_UScriptStruct_FHarmonyVariantProbeOverride(ETypeConstructPhase::Inner); }


struct FHarmonyVariantProbeOverride;
// ********** End ScriptStruct FHarmonyVariantProbeOverride ****************************************

// ********** Begin ScriptStruct FHarmonyCaptureJobSettings ****************************************
struct Z_Construct_UScriptStruct_FHarmonyCaptureJobSettings_Statics;
HARMONYCAPTURE_API UScriptStruct* Z_Construct_UScriptStruct_FHarmonyCaptureJobSettings(ETypeConstructPhase);

#define FID_HostProject_Plugins_Harmony_Source_HarmonyCapture_Public_HarmonyCaptureRig_h_104_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FHarmonyCaptureJobSettings_Statics; \
	UE_NODEBUG static UScriptStruct* StaticStruct() { return Z_Construct_UScriptStruct_FHarmonyCaptureJobSettings(ETypeConstructPhase::Inner); }


struct FHarmonyCaptureJobSettings;
// ********** End ScriptStruct FHarmonyCaptureJobSettings ******************************************

// ********** Begin ScriptStruct FHarmonyCaptureCustomView *****************************************
struct Z_Construct_UScriptStruct_FHarmonyCaptureCustomView_Statics;
HARMONYCAPTURE_API UScriptStruct* Z_Construct_UScriptStruct_FHarmonyCaptureCustomView(ETypeConstructPhase);

#define FID_HostProject_Plugins_Harmony_Source_HarmonyCapture_Public_HarmonyCaptureRig_h_190_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FHarmonyCaptureCustomView_Statics; \
	UE_NODEBUG static UScriptStruct* StaticStruct() { return Z_Construct_UScriptStruct_FHarmonyCaptureCustomView(ETypeConstructPhase::Inner); }


struct FHarmonyCaptureCustomView;
// ********** End ScriptStruct FHarmonyCaptureCustomView *******************************************

// ********** Begin Class AHarmonyCaptureRig *******************************************************
#define FID_HostProject_Plugins_Harmony_Source_HarmonyCapture_Public_HarmonyCaptureRig_h_244_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execGetShotsPerLocation); \
	DECLARE_FUNCTION(execSetShotIndex); \
	DECLARE_FUNCTION(execGenerateCameraSamples); \
	DECLARE_FUNCTION(execCancelCapture); \
	DECLARE_FUNCTION(execBuildFallbackFromLastCapture); \
	DECLARE_FUNCTION(execRetrainFromLastCapture); \
	DECLARE_FUNCTION(execBakeVariants); \
	DECLARE_FUNCTION(execCaptureOnly); \
	DECLARE_FUNCTION(execCaptureAndTrain); \
	DECLARE_FUNCTION(execGetExcludedActors); \
	DECLARE_FUNCTION(execGetCustomShotPose); \
	DECLARE_FUNCTION(execGetNumCustomShots); \
	DECLARE_FUNCTION(execAddCustomView); \
	DECLARE_FUNCTION(execCaptureCurrentView);


struct Z_Construct_UClass_AHarmonyCaptureRig_Statics;
HARMONYCAPTURE_API UClass* Z_Construct_UClass_AHarmonyCaptureRig(ETypeConstructPhase);

#define FID_HostProject_Plugins_Harmony_Source_HarmonyCapture_Public_HarmonyCaptureRig_h_244_INCLASS_NO_PURE_DECLS \
private: \
	friend struct ::Z_Construct_UClass_AHarmonyCaptureRig_Statics; \
	friend HARMONYCAPTURE_API UClass* ::Z_Construct_UClass_AHarmonyCaptureRig(ETypeConstructPhase); \
public: \
	DECLARE_CLASS2(AHarmonyCaptureRig, AActor, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/HarmonyCapture"), Z_Construct_UClass_AHarmonyCaptureRig) \
	DECLARE_SERIALIZER(AHarmonyCaptureRig)


#define FID_HostProject_Plugins_Harmony_Source_HarmonyCapture_Public_HarmonyCaptureRig_h_244_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	AHarmonyCaptureRig(AHarmonyCaptureRig&&) = delete; \
	AHarmonyCaptureRig(const AHarmonyCaptureRig&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AHarmonyCaptureRig); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AHarmonyCaptureRig); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(AHarmonyCaptureRig) \
	NO_API virtual ~AHarmonyCaptureRig();


#define FID_HostProject_Plugins_Harmony_Source_HarmonyCapture_Public_HarmonyCaptureRig_h_241_PROLOG
#define FID_HostProject_Plugins_Harmony_Source_HarmonyCapture_Public_HarmonyCaptureRig_h_244_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_HostProject_Plugins_Harmony_Source_HarmonyCapture_Public_HarmonyCaptureRig_h_244_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_HostProject_Plugins_Harmony_Source_HarmonyCapture_Public_HarmonyCaptureRig_h_244_INCLASS_NO_PURE_DECLS \
	FID_HostProject_Plugins_Harmony_Source_HarmonyCapture_Public_HarmonyCaptureRig_h_244_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class AHarmonyCaptureRig;

// ********** End Class AHarmonyCaptureRig *********************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_HostProject_Plugins_Harmony_Source_HarmonyCapture_Public_HarmonyCaptureRig_h

// ********** Begin Enum EHarmonyCaptureRadiusMode *************************************************
#define FOREACH_ENUM_EHARMONYCAPTURERADIUSMODE(op) \
	op(EHarmonyCaptureRadiusMode::ProjectDefault) \
	op(EHarmonyCaptureRadiusMode::Infinite) \
	op(EHarmonyCaptureRadiusMode::Custom) 

enum class EHarmonyCaptureRadiusMode : uint8;
template<> struct TIsUEnumClass<EHarmonyCaptureRadiusMode> { enum { Value = true }; };
template<> UE_NODEBUG HARMONYCAPTURE_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyCaptureRadiusMode>();
// ********** End Enum EHarmonyCaptureRadiusMode ***************************************************

// ********** Begin Enum EHarmonyCaptureShotSet ****************************************************
#define FOREACH_ENUM_EHARMONYCAPTURESHOTSET(op) \
	op(EHarmonyCaptureShotSet::All) \
	op(EHarmonyCaptureShotSet::SamplesOnly) \
	op(EHarmonyCaptureShotSet::CustomViewsOnly) 

enum class EHarmonyCaptureShotSet : uint8;
template<> struct TIsUEnumClass<EHarmonyCaptureShotSet> { enum { Value = true }; };
template<> UE_NODEBUG HARMONYCAPTURE_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyCaptureShotSet>();
// ********** End Enum EHarmonyCaptureShotSet ******************************************************

// ********** Begin Enum EHarmonyCaptureExposure ***************************************************
#define FOREACH_ENUM_EHARMONYCAPTUREEXPOSURE(op) \
	op(EHarmonyCaptureExposure::Auto) \
	op(EHarmonyCaptureExposure::Locked) \
	op(EHarmonyCaptureExposure::Scene) 

enum class EHarmonyCaptureExposure : uint8;
template<> struct TIsUEnumClass<EHarmonyCaptureExposure> { enum { Value = true }; };
template<> UE_NODEBUG HARMONYCAPTURE_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyCaptureExposure>();
// ********** End Enum EHarmonyCaptureExposure *****************************************************

// ********** Begin Enum EHarmonyVariantProbeStore *************************************************
#define FOREACH_ENUM_EHARMONYVARIANTPROBESTORE(op) \
	op(EHarmonyVariantProbeStore::Auto) \
	op(EHarmonyVariantProbeStore::AlwaysStore) \
	op(EHarmonyVariantProbeStore::NeverStore) 

enum class EHarmonyVariantProbeStore : uint8;
template<> struct TIsUEnumClass<EHarmonyVariantProbeStore> { enum { Value = true }; };
template<> UE_NODEBUG HARMONYCAPTURE_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyVariantProbeStore>();
// ********** End Enum EHarmonyVariantProbeStore ***************************************************

// ********** Begin Enum EHarmonyFloorReflectionBake ***********************************************
#define FOREACH_ENUM_EHARMONYFLOORREFLECTIONBAKE(op) \
	op(EHarmonyFloorReflectionBake::Auto) \
	op(EHarmonyFloorReflectionBake::Always) \
	op(EHarmonyFloorReflectionBake::Never) 

enum class EHarmonyFloorReflectionBake : uint8;
template<> struct TIsUEnumClass<EHarmonyFloorReflectionBake> { enum { Value = true }; };
template<> UE_NODEBUG HARMONYCAPTURE_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyFloorReflectionBake>();
// ********** End Enum EHarmonyFloorReflectionBake *************************************************

// ********** Begin Enum EHarmonyCaptureSamplingPattern ********************************************
#define FOREACH_ENUM_EHARMONYCAPTURESAMPLINGPATTERN(op) \
	op(EHarmonyCaptureSamplingPattern::Grid) \
	op(EHarmonyCaptureSamplingPattern::Radial) \
	op(EHarmonyCaptureSamplingPattern::Rows) 

enum class EHarmonyCaptureSamplingPattern : uint8;
template<> struct TIsUEnumClass<EHarmonyCaptureSamplingPattern> { enum { Value = true }; };
template<> UE_NODEBUG HARMONYCAPTURE_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyCaptureSamplingPattern>();
// ********** End Enum EHarmonyCaptureSamplingPattern **********************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
