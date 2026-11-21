// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "HarmonyFileLibrary.h"

#ifdef HARMONYEDITOR_HarmonyFileLibrary_generated_h
#error "HarmonyFileLibrary.generated.h already included, missing '#pragma once' in HarmonyFileLibrary.h"
#endif
#define HARMONYEDITOR_HarmonyFileLibrary_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class AHarmonyActor;
class UHarmonyDataAsset;

// ********** Begin Class UHarmonyFileLibrary ******************************************************
#define FID_HostProject_Plugins_Harmony_Source_HarmonyEditor_Public_HarmonyFileLibrary_h_15_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execImportHarmonyFileIntoLevel); \
	DECLARE_FUNCTION(execImportHarmonyFile); \
	DECLARE_FUNCTION(execExportHarmonyActor); \
	DECLARE_FUNCTION(execExportHarmonyAsset);


struct Z_Construct_UClass_UHarmonyFileLibrary_Statics;
HARMONYEDITOR_API UClass* Z_Construct_UClass_UHarmonyFileLibrary(ETypeConstructPhase);

#define FID_HostProject_Plugins_Harmony_Source_HarmonyEditor_Public_HarmonyFileLibrary_h_15_INCLASS_NO_PURE_DECLS \
private: \
	friend struct ::Z_Construct_UClass_UHarmonyFileLibrary_Statics; \
	friend HARMONYEDITOR_API UClass* ::Z_Construct_UClass_UHarmonyFileLibrary(ETypeConstructPhase); \
public: \
	DECLARE_CLASS2(UHarmonyFileLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/HarmonyEditor"), Z_Construct_UClass_UHarmonyFileLibrary) \
	DECLARE_SERIALIZER(UHarmonyFileLibrary)


#define FID_HostProject_Plugins_Harmony_Source_HarmonyEditor_Public_HarmonyFileLibrary_h_15_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UHarmonyFileLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	/** Deleted move- and copy-constructors, should never be used */ \
	UHarmonyFileLibrary(UHarmonyFileLibrary&&) = delete; \
	UHarmonyFileLibrary(const UHarmonyFileLibrary&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UHarmonyFileLibrary); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UHarmonyFileLibrary); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UHarmonyFileLibrary) \
	NO_API virtual ~UHarmonyFileLibrary();


#define FID_HostProject_Plugins_Harmony_Source_HarmonyEditor_Public_HarmonyFileLibrary_h_12_PROLOG
#define FID_HostProject_Plugins_Harmony_Source_HarmonyEditor_Public_HarmonyFileLibrary_h_15_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_HostProject_Plugins_Harmony_Source_HarmonyEditor_Public_HarmonyFileLibrary_h_15_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_HostProject_Plugins_Harmony_Source_HarmonyEditor_Public_HarmonyFileLibrary_h_15_INCLASS_NO_PURE_DECLS \
	FID_HostProject_Plugins_Harmony_Source_HarmonyEditor_Public_HarmonyFileLibrary_h_15_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UHarmonyFileLibrary;

// ********** End Class UHarmonyFileLibrary ********************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_HostProject_Plugins_Harmony_Source_HarmonyEditor_Public_HarmonyFileLibrary_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
