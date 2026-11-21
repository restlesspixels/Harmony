// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "HarmonyFallbackBuildSettings.h"

#ifdef HARMONYFALLBACKEDITOR_HarmonyFallbackBuildSettings_generated_h
#error "HarmonyFallbackBuildSettings.generated.h already included, missing '#pragma once' in HarmonyFallbackBuildSettings.h"
#endif
#define HARMONYFALLBACKEDITOR_HarmonyFallbackBuildSettings_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class UHarmonyFallbackBuildSettings ********************************************
struct Z_Construct_UClass_UHarmonyFallbackBuildSettings_Statics;
HARMONYFALLBACKEDITOR_API UClass* Z_Construct_UClass_UHarmonyFallbackBuildSettings(ETypeConstructPhase);

#define FID_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackBuildSettings_h_17_INCLASS_NO_PURE_DECLS \
private: \
	friend struct ::Z_Construct_UClass_UHarmonyFallbackBuildSettings_Statics; \
	friend HARMONYFALLBACKEDITOR_API UClass* ::Z_Construct_UClass_UHarmonyFallbackBuildSettings(ETypeConstructPhase); \
public: \
	DECLARE_CLASS2(UHarmonyFallbackBuildSettings, UDeveloperSettings, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/HarmonyFallbackEditor"), Z_Construct_UClass_UHarmonyFallbackBuildSettings) \
	DECLARE_SERIALIZER(UHarmonyFallbackBuildSettings) \
	static constexpr const TCHAR* StaticConfigName() {return TEXT("EditorPerProjectUserSettings");} \



#define FID_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackBuildSettings_h_17_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UHarmonyFallbackBuildSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	/** Deleted move- and copy-constructors, should never be used */ \
	UHarmonyFallbackBuildSettings(UHarmonyFallbackBuildSettings&&) = delete; \
	UHarmonyFallbackBuildSettings(const UHarmonyFallbackBuildSettings&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UHarmonyFallbackBuildSettings); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UHarmonyFallbackBuildSettings); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UHarmonyFallbackBuildSettings) \
	NO_API virtual ~UHarmonyFallbackBuildSettings();


#define FID_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackBuildSettings_h_14_PROLOG
#define FID_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackBuildSettings_h_17_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackBuildSettings_h_17_INCLASS_NO_PURE_DECLS \
	FID_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackBuildSettings_h_17_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UHarmonyFallbackBuildSettings;

// ********** End Class UHarmonyFallbackBuildSettings **********************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_HostProject_Plugins_Harmony_Source_HarmonyFallbackEditor_Public_HarmonyFallbackBuildSettings_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
