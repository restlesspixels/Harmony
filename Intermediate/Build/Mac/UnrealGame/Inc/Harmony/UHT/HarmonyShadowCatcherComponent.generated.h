// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "HarmonyShadowCatcherComponent.h"

#ifdef HARMONY_HarmonyShadowCatcherComponent_generated_h
#error "HarmonyShadowCatcherComponent.generated.h already included, missing '#pragma once' in HarmonyShadowCatcherComponent.h"
#endif
#define HARMONY_HarmonyShadowCatcherComponent_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "Templates/IsUEnumClass.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class UHarmonyShadowCatcherComponent *******************************************
#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyShadowCatcherComponent_h_50_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execUpdateShadows); \
	DECLARE_FUNCTION(execRefreshCasters); \
	DECLARE_FUNCTION(execSetCatchShadows); \
	DECLARE_FUNCTION(execSetCatchReflections);


struct Z_Construct_UClass_UHarmonyShadowCatcherComponent_Statics;
HARMONY_API UClass* Z_Construct_UClass_UHarmonyShadowCatcherComponent(ETypeConstructPhase);

#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyShadowCatcherComponent_h_50_INCLASS_NO_PURE_DECLS \
private: \
	friend struct ::Z_Construct_UClass_UHarmonyShadowCatcherComponent_Statics; \
	friend HARMONY_API UClass* ::Z_Construct_UClass_UHarmonyShadowCatcherComponent(ETypeConstructPhase); \
public: \
	DECLARE_CLASS2(UHarmonyShadowCatcherComponent, UActorComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/Harmony"), Z_Construct_UClass_UHarmonyShadowCatcherComponent) \
	DECLARE_SERIALIZER(UHarmonyShadowCatcherComponent)


#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyShadowCatcherComponent_h_50_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	UHarmonyShadowCatcherComponent(UHarmonyShadowCatcherComponent&&) = delete; \
	UHarmonyShadowCatcherComponent(const UHarmonyShadowCatcherComponent&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UHarmonyShadowCatcherComponent); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UHarmonyShadowCatcherComponent); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UHarmonyShadowCatcherComponent) \
	NO_API virtual ~UHarmonyShadowCatcherComponent();


#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyShadowCatcherComponent_h_47_PROLOG
#define FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyShadowCatcherComponent_h_50_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyShadowCatcherComponent_h_50_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyShadowCatcherComponent_h_50_INCLASS_NO_PURE_DECLS \
	FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyShadowCatcherComponent_h_50_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UHarmonyShadowCatcherComponent;

// ********** End Class UHarmonyShadowCatcherComponent *********************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_jellyfin_harmony_precompiled_work_5_9_Mac_pkg_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyShadowCatcherComponent_h

// ********** Begin Enum EHarmonyShadowGround ******************************************************
#define FOREACH_ENUM_EHARMONYSHADOWGROUND(op) \
	op(EHarmonyShadowGround::Auto) \
	op(EHarmonyShadowGround::Plane) 

enum class EHarmonyShadowGround : uint8;
template<> struct TIsUEnumClass<EHarmonyShadowGround> { enum { Value = true }; };
template<> UE_NODEBUG HARMONY_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyShadowGround>();
// ********** End Enum EHarmonyShadowGround ********************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
