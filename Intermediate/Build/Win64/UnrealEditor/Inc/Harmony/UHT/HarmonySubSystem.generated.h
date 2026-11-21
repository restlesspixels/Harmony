// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "HarmonySubSystem.h"

#ifdef HARMONY_HarmonySubSystem_generated_h
#error "HarmonySubSystem.generated.h already included, missing '#pragma once' in HarmonySubSystem.h"
#endif
#define HARMONY_HarmonySubSystem_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class UHarmonySubsystem ********************************************************
struct Z_Construct_UClass_UHarmonySubsystem_Statics;
HARMONY_API UClass* Z_Construct_UClass_UHarmonySubsystem(ETypeConstructPhase);

#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonySubSystem_h_15_INCLASS_NO_PURE_DECLS \
private: \
	friend struct ::Z_Construct_UClass_UHarmonySubsystem_Statics; \
	friend HARMONY_API UClass* ::Z_Construct_UClass_UHarmonySubsystem(ETypeConstructPhase); \
public: \
	DECLARE_CLASS2(UHarmonySubsystem, UEngineSubsystem, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/Harmony"), Z_Construct_UClass_UHarmonySubsystem) \
	DECLARE_SERIALIZER(UHarmonySubsystem)


#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonySubSystem_h_15_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UHarmonySubsystem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	/** Deleted move- and copy-constructors, should never be used */ \
	UHarmonySubsystem(UHarmonySubsystem&&) = delete; \
	UHarmonySubsystem(const UHarmonySubsystem&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UHarmonySubsystem); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UHarmonySubsystem); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UHarmonySubsystem) \
	NO_API virtual ~UHarmonySubsystem();


#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonySubSystem_h_12_PROLOG
#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonySubSystem_h_15_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonySubSystem_h_15_INCLASS_NO_PURE_DECLS \
	FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonySubSystem_h_15_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UHarmonySubsystem;

// ********** End Class UHarmonySubsystem **********************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonySubSystem_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
