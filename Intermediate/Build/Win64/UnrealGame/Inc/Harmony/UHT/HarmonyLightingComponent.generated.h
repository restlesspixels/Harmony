// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "HarmonyLightingComponent.h"

#ifdef HARMONY_HarmonyLightingComponent_generated_h
#error "HarmonyLightingComponent.generated.h already included, missing '#pragma once' in HarmonyLightingComponent.h"
#endif
#define HARMONY_HarmonyLightingComponent_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "Templates/IsUEnumClass.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
enum class EHarmonyLightScene : uint8;

// ********** Begin Class UHarmonyLightingComponent ************************************************
#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyLightingComponent_h_56_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execUpdateLighting); \
	DECLARE_FUNCTION(execSetLightScene);


struct Z_Construct_UClass_UHarmonyLightingComponent_Statics;
HARMONY_API UClass* Z_Construct_UClass_UHarmonyLightingComponent(ETypeConstructPhase);

#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyLightingComponent_h_56_INCLASS_NO_PURE_DECLS \
private: \
	friend struct ::Z_Construct_UClass_UHarmonyLightingComponent_Statics; \
	friend HARMONY_API UClass* ::Z_Construct_UClass_UHarmonyLightingComponent(ETypeConstructPhase); \
public: \
	DECLARE_CLASS2(UHarmonyLightingComponent, UActorComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/Harmony"), Z_Construct_UClass_UHarmonyLightingComponent) \
	DECLARE_SERIALIZER(UHarmonyLightingComponent)


#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyLightingComponent_h_56_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	UHarmonyLightingComponent(UHarmonyLightingComponent&&) = delete; \
	UHarmonyLightingComponent(const UHarmonyLightingComponent&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UHarmonyLightingComponent); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UHarmonyLightingComponent); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UHarmonyLightingComponent) \
	NO_API virtual ~UHarmonyLightingComponent();


#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyLightingComponent_h_53_PROLOG
#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyLightingComponent_h_56_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyLightingComponent_h_56_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyLightingComponent_h_56_INCLASS_NO_PURE_DECLS \
	FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyLightingComponent_h_56_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UHarmonyLightingComponent;

// ********** End Class UHarmonyLightingComponent **************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyLightingComponent_h

// ********** Begin Enum EHarmonyLightScene ********************************************************
#define FOREACH_ENUM_EHARMONYLIGHTSCENE(op) \
	op(EHarmonyLightScene::Auto) \
	op(EHarmonyLightScene::Always) \
	op(EHarmonyLightScene::Off) 

enum class EHarmonyLightScene : uint8;
template<> struct TIsUEnumClass<EHarmonyLightScene> { enum { Value = true }; };
template<> UE_NODEBUG HARMONY_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyLightScene>();
// ********** End Enum EHarmonyLightScene **********************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
