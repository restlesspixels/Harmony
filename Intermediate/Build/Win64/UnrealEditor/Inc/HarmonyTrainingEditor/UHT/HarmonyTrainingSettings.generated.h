// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "HarmonyTrainingSettings.h"

#ifdef HARMONYTRAININGEDITOR_HarmonyTrainingSettings_generated_h
#error "HarmonyTrainingSettings.generated.h already included, missing '#pragma once' in HarmonyTrainingSettings.h"
#endif
#define HARMONYTRAININGEDITOR_HarmonyTrainingSettings_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class UHarmonyTrainingSettings *************************************************
struct Z_Construct_UClass_UHarmonyTrainingSettings_Statics;
HARMONYTRAININGEDITOR_API UClass* Z_Construct_UClass_UHarmonyTrainingSettings(ETypeConstructPhase);

#define FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingSettings_h_17_INCLASS_NO_PURE_DECLS \
private: \
	friend struct ::Z_Construct_UClass_UHarmonyTrainingSettings_Statics; \
	friend HARMONYTRAININGEDITOR_API UClass* ::Z_Construct_UClass_UHarmonyTrainingSettings(ETypeConstructPhase); \
public: \
	DECLARE_CLASS2(UHarmonyTrainingSettings, UDeveloperSettings, COMPILED_IN_FLAGS(0 | CLASS_DefaultConfig | CLASS_Config), CASTCLASS_None, TEXT("/Script/HarmonyTrainingEditor"), Z_Construct_UClass_UHarmonyTrainingSettings) \
	DECLARE_SERIALIZER(UHarmonyTrainingSettings) \
	static constexpr const TCHAR* StaticConfigName() {return TEXT("Game");} \



#define FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingSettings_h_17_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	UHarmonyTrainingSettings(UHarmonyTrainingSettings&&) = delete; \
	UHarmonyTrainingSettings(const UHarmonyTrainingSettings&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UHarmonyTrainingSettings); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UHarmonyTrainingSettings); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UHarmonyTrainingSettings) \
	NO_API virtual ~UHarmonyTrainingSettings();


#define FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingSettings_h_14_PROLOG
#define FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingSettings_h_17_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingSettings_h_17_INCLASS_NO_PURE_DECLS \
	FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingSettings_h_17_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UHarmonyTrainingSettings;

// ********** End Class UHarmonyTrainingSettings ***************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_HostProject_Plugins_Harmony_Source_HarmonyTrainingEditor_Public_HarmonyTrainingSettings_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
