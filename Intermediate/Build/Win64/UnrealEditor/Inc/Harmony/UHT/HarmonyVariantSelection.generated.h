// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "HarmonyVariantSelection.h"

#ifdef HARMONY_HarmonyVariantSelection_generated_h
#error "HarmonyVariantSelection.generated.h already included, missing '#pragma once' in HarmonyVariantSelection.h"
#endif
#define HARMONY_HarmonyVariantSelection_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "Templates/IsUEnumClass.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class UHarmonyVariantSelection *************************************************
#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyVariantSelection_h_173_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execIsSlot4Shown); \
	DECLARE_FUNCTION(execIsSlot3Shown); \
	DECLARE_FUNCTION(execIsSlot2Shown); \
	DECLARE_FUNCTION(execIsSlot1Shown); \
	DECLARE_FUNCTION(execIsTransitionShown); \
	DECLARE_FUNCTION(execSelectVariant); \
	DECLARE_FUNCTION(execGetSelectedVariant); \
	DECLARE_FUNCTION(execGetNumVariants); \
	DECLARE_FUNCTION(execGetNumVariantSets);


struct Z_Construct_UClass_UHarmonyVariantSelection_Statics;
HARMONY_API UClass* Z_Construct_UClass_UHarmonyVariantSelection(ETypeConstructPhase);

#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyVariantSelection_h_173_INCLASS_NO_PURE_DECLS \
private: \
	friend struct ::Z_Construct_UClass_UHarmonyVariantSelection_Statics; \
	friend HARMONY_API UClass* ::Z_Construct_UClass_UHarmonyVariantSelection(ETypeConstructPhase); \
public: \
	DECLARE_CLASS2(UHarmonyVariantSelection, UObject, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/Harmony"), Z_Construct_UClass_UHarmonyVariantSelection) \
	DECLARE_SERIALIZER(UHarmonyVariantSelection)


#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyVariantSelection_h_173_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UHarmonyVariantSelection(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	/** Deleted move- and copy-constructors, should never be used */ \
	UHarmonyVariantSelection(UHarmonyVariantSelection&&) = delete; \
	UHarmonyVariantSelection(const UHarmonyVariantSelection&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UHarmonyVariantSelection); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UHarmonyVariantSelection); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UHarmonyVariantSelection) \
	NO_API virtual ~UHarmonyVariantSelection();


#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyVariantSelection_h_170_PROLOG
#define FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyVariantSelection_h_173_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyVariantSelection_h_173_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyVariantSelection_h_173_INCLASS_NO_PURE_DECLS \
	FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyVariantSelection_h_173_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UHarmonyVariantSelection;

// ********** End Class UHarmonyVariantSelection ***************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyVariantSelection_h

// ********** Begin Enum EHarmonyVariantSlot1 ******************************************************
#define FOREACH_ENUM_EHARMONYVARIANTSLOT1(op) \
	op(EHarmonyVariantSlot1::Variant1) \
	op(EHarmonyVariantSlot1::Variant2) \
	op(EHarmonyVariantSlot1::Variant3) \
	op(EHarmonyVariantSlot1::Variant4) \
	op(EHarmonyVariantSlot1::Variant5) \
	op(EHarmonyVariantSlot1::Variant6) \
	op(EHarmonyVariantSlot1::Variant7) \
	op(EHarmonyVariantSlot1::Variant8) \
	op(EHarmonyVariantSlot1::Variant9) \
	op(EHarmonyVariantSlot1::Variant10) \
	op(EHarmonyVariantSlot1::Variant11) \
	op(EHarmonyVariantSlot1::Variant12) \
	op(EHarmonyVariantSlot1::Variant13) \
	op(EHarmonyVariantSlot1::Variant14) \
	op(EHarmonyVariantSlot1::Variant15) \
	op(EHarmonyVariantSlot1::Variant16) \
	op(EHarmonyVariantSlot1::Variant17) \
	op(EHarmonyVariantSlot1::Variant18) \
	op(EHarmonyVariantSlot1::Variant19) \
	op(EHarmonyVariantSlot1::Variant20) \
	op(EHarmonyVariantSlot1::Variant21) \
	op(EHarmonyVariantSlot1::Variant22) \
	op(EHarmonyVariantSlot1::Variant23) \
	op(EHarmonyVariantSlot1::Variant24) \
	op(EHarmonyVariantSlot1::Variant25) \
	op(EHarmonyVariantSlot1::Variant26) \
	op(EHarmonyVariantSlot1::Variant27) \
	op(EHarmonyVariantSlot1::Variant28) \
	op(EHarmonyVariantSlot1::Variant29) \
	op(EHarmonyVariantSlot1::Variant30) \
	op(EHarmonyVariantSlot1::Variant31) \
	op(EHarmonyVariantSlot1::Variant32) 

enum class EHarmonyVariantSlot1 : uint8;
template<> struct TIsUEnumClass<EHarmonyVariantSlot1> { enum { Value = true }; };
template<> UE_NODEBUG HARMONY_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyVariantSlot1>();
// ********** End Enum EHarmonyVariantSlot1 ********************************************************

// ********** Begin Enum EHarmonyVariantSlot2 ******************************************************
#define FOREACH_ENUM_EHARMONYVARIANTSLOT2(op) \
	op(EHarmonyVariantSlot2::Variant1) \
	op(EHarmonyVariantSlot2::Variant2) \
	op(EHarmonyVariantSlot2::Variant3) \
	op(EHarmonyVariantSlot2::Variant4) \
	op(EHarmonyVariantSlot2::Variant5) \
	op(EHarmonyVariantSlot2::Variant6) \
	op(EHarmonyVariantSlot2::Variant7) \
	op(EHarmonyVariantSlot2::Variant8) \
	op(EHarmonyVariantSlot2::Variant9) \
	op(EHarmonyVariantSlot2::Variant10) \
	op(EHarmonyVariantSlot2::Variant11) \
	op(EHarmonyVariantSlot2::Variant12) \
	op(EHarmonyVariantSlot2::Variant13) \
	op(EHarmonyVariantSlot2::Variant14) \
	op(EHarmonyVariantSlot2::Variant15) \
	op(EHarmonyVariantSlot2::Variant16) \
	op(EHarmonyVariantSlot2::Variant17) \
	op(EHarmonyVariantSlot2::Variant18) \
	op(EHarmonyVariantSlot2::Variant19) \
	op(EHarmonyVariantSlot2::Variant20) \
	op(EHarmonyVariantSlot2::Variant21) \
	op(EHarmonyVariantSlot2::Variant22) \
	op(EHarmonyVariantSlot2::Variant23) \
	op(EHarmonyVariantSlot2::Variant24) \
	op(EHarmonyVariantSlot2::Variant25) \
	op(EHarmonyVariantSlot2::Variant26) \
	op(EHarmonyVariantSlot2::Variant27) \
	op(EHarmonyVariantSlot2::Variant28) \
	op(EHarmonyVariantSlot2::Variant29) \
	op(EHarmonyVariantSlot2::Variant30) \
	op(EHarmonyVariantSlot2::Variant31) \
	op(EHarmonyVariantSlot2::Variant32) 

enum class EHarmonyVariantSlot2 : uint8;
template<> struct TIsUEnumClass<EHarmonyVariantSlot2> { enum { Value = true }; };
template<> UE_NODEBUG HARMONY_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyVariantSlot2>();
// ********** End Enum EHarmonyVariantSlot2 ********************************************************

// ********** Begin Enum EHarmonyVariantSlot3 ******************************************************
#define FOREACH_ENUM_EHARMONYVARIANTSLOT3(op) \
	op(EHarmonyVariantSlot3::Variant1) \
	op(EHarmonyVariantSlot3::Variant2) \
	op(EHarmonyVariantSlot3::Variant3) \
	op(EHarmonyVariantSlot3::Variant4) \
	op(EHarmonyVariantSlot3::Variant5) \
	op(EHarmonyVariantSlot3::Variant6) \
	op(EHarmonyVariantSlot3::Variant7) \
	op(EHarmonyVariantSlot3::Variant8) \
	op(EHarmonyVariantSlot3::Variant9) \
	op(EHarmonyVariantSlot3::Variant10) \
	op(EHarmonyVariantSlot3::Variant11) \
	op(EHarmonyVariantSlot3::Variant12) \
	op(EHarmonyVariantSlot3::Variant13) \
	op(EHarmonyVariantSlot3::Variant14) \
	op(EHarmonyVariantSlot3::Variant15) \
	op(EHarmonyVariantSlot3::Variant16) \
	op(EHarmonyVariantSlot3::Variant17) \
	op(EHarmonyVariantSlot3::Variant18) \
	op(EHarmonyVariantSlot3::Variant19) \
	op(EHarmonyVariantSlot3::Variant20) \
	op(EHarmonyVariantSlot3::Variant21) \
	op(EHarmonyVariantSlot3::Variant22) \
	op(EHarmonyVariantSlot3::Variant23) \
	op(EHarmonyVariantSlot3::Variant24) \
	op(EHarmonyVariantSlot3::Variant25) \
	op(EHarmonyVariantSlot3::Variant26) \
	op(EHarmonyVariantSlot3::Variant27) \
	op(EHarmonyVariantSlot3::Variant28) \
	op(EHarmonyVariantSlot3::Variant29) \
	op(EHarmonyVariantSlot3::Variant30) \
	op(EHarmonyVariantSlot3::Variant31) \
	op(EHarmonyVariantSlot3::Variant32) 

enum class EHarmonyVariantSlot3 : uint8;
template<> struct TIsUEnumClass<EHarmonyVariantSlot3> { enum { Value = true }; };
template<> UE_NODEBUG HARMONY_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyVariantSlot3>();
// ********** End Enum EHarmonyVariantSlot3 ********************************************************

// ********** Begin Enum EHarmonyVariantSlot4 ******************************************************
#define FOREACH_ENUM_EHARMONYVARIANTSLOT4(op) \
	op(EHarmonyVariantSlot4::Variant1) \
	op(EHarmonyVariantSlot4::Variant2) \
	op(EHarmonyVariantSlot4::Variant3) \
	op(EHarmonyVariantSlot4::Variant4) \
	op(EHarmonyVariantSlot4::Variant5) \
	op(EHarmonyVariantSlot4::Variant6) \
	op(EHarmonyVariantSlot4::Variant7) \
	op(EHarmonyVariantSlot4::Variant8) \
	op(EHarmonyVariantSlot4::Variant9) \
	op(EHarmonyVariantSlot4::Variant10) \
	op(EHarmonyVariantSlot4::Variant11) \
	op(EHarmonyVariantSlot4::Variant12) \
	op(EHarmonyVariantSlot4::Variant13) \
	op(EHarmonyVariantSlot4::Variant14) \
	op(EHarmonyVariantSlot4::Variant15) \
	op(EHarmonyVariantSlot4::Variant16) \
	op(EHarmonyVariantSlot4::Variant17) \
	op(EHarmonyVariantSlot4::Variant18) \
	op(EHarmonyVariantSlot4::Variant19) \
	op(EHarmonyVariantSlot4::Variant20) \
	op(EHarmonyVariantSlot4::Variant21) \
	op(EHarmonyVariantSlot4::Variant22) \
	op(EHarmonyVariantSlot4::Variant23) \
	op(EHarmonyVariantSlot4::Variant24) \
	op(EHarmonyVariantSlot4::Variant25) \
	op(EHarmonyVariantSlot4::Variant26) \
	op(EHarmonyVariantSlot4::Variant27) \
	op(EHarmonyVariantSlot4::Variant28) \
	op(EHarmonyVariantSlot4::Variant29) \
	op(EHarmonyVariantSlot4::Variant30) \
	op(EHarmonyVariantSlot4::Variant31) \
	op(EHarmonyVariantSlot4::Variant32) 

enum class EHarmonyVariantSlot4 : uint8;
template<> struct TIsUEnumClass<EHarmonyVariantSlot4> { enum { Value = true }; };
template<> UE_NODEBUG HARMONY_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyVariantSlot4>();
// ********** End Enum EHarmonyVariantSlot4 ********************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
