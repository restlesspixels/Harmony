// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HarmonyVariantSource.generated.h"

class UMaterialInterface;
class UMeshComponent;

/** P10: one variant set (slot) a variant source offers: its options and which one the scene shows now. */
USTRUCT(BlueprintType)
struct HARMONYCAPTURE_API FHarmonyCaptureVariantSet
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    FName Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    TArray<FName> Options;

    /** The option the scene shows now (the base capture's option; NAME_None when unknown). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    FName CurrentOption;
};

/**
 * P10 configurable variants: where the capture rig gets the scene's configuration options from. The rig captures the
 * base scene, then, per option of each variant set, applies it here, captures again with the same poses and bakes the
 * difference into the Harmony asset as that option. Harmony itself knows nothing about configuration systems: a source
 * only lists sets and options and applies one. Built in: UHarmonyMaterialVariantSource (material swaps, no other system)
 * and, in the HarmonyVariantManager module, UHarmonyLevelVariantSetsSource (Unreal's Variant Manager). Any other
 * configurator can subclass this in C++ or Blueprint.
 *
 * Options of different sets should be independent (a wall finish and a car paint), so N sets of K options need N*K
 * captures, not K^N. Light that one option bounces onto another set's objects is captured against the base of the other
 * sets only.
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced, CollapseCategories)
class HARMONYCAPTURE_API UHarmonyVariantSource : public UObject
{
    GENERATED_BODY()

public:
    /** The variant sets with their options and current option. */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Harmony|Variants")
    TArray<FHarmonyCaptureVariantSet> GetVariantSets(UObject* WorldContextObject) const;

    /** Makes the scene show Option of variant set SetName; false when the set or option is unknown. */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Harmony|Variants")
    bool ApplyVariant(UObject* WorldContextObject, FName SetName, FName OptionName);

    virtual TArray<FHarmonyCaptureVariantSet> GetVariantSets_Implementation(UObject* WorldContextObject) const { return {}; }
    virtual bool ApplyVariant_Implementation(UObject* WorldContextObject, FName SetName, FName OptionName) { return false; }
};

/** One material swap of a material-swap option. */
USTRUCT(BlueprintType)
struct HARMONYCAPTURE_API FHarmonyMaterialSwap
{
    GENERATED_BODY()

    /** The actor whose mesh components get the material. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    TSoftObjectPtr<AActor> Actor;

    /** Material slot (INDEX_NONE / -1: every slot whose current material name contains MatchMaterialName, or every slot
        when that is empty). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    int32 SlotIndex = -1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    FString MatchMaterialName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    TObjectPtr<UMaterialInterface> Material;
};

USTRUCT(BlueprintType)
struct HARMONYCAPTURE_API FHarmonyMaterialVariantOption
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    FName Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    TArray<FHarmonyMaterialSwap> Swaps;
};

USTRUCT(BlueprintType)
struct HARMONYCAPTURE_API FHarmonyMaterialVariantSet
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    FName Name;

    /** What the scene shows as authored (applying it restores the materials found before the first swap). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    FName BaseOption = TEXT("Base");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    TArray<FHarmonyMaterialVariantOption> Options;
};

/** A material slot a material variant swap replaced (UHarmonyMaterialVariantSource restores it). */
USTRUCT()
struct HARMONYCAPTURE_API FHarmonyMaterialSavedSlot
{
    GENERATED_BODY()

    UPROPERTY(Transient)
    TWeakObjectPtr<UMeshComponent> Component;

    UPROPERTY(Transient)
    int32 Slot = 0;

    /** The slot's own override before the swap (static and skinned meshes; nullptr: none, the mesh's material shows),
        or the material it showed (other mesh components, whose SetMaterial may not use OverrideMaterials). */
    UPROPERTY(Transient)
    TObjectPtr<UMaterialInterface> Material;

    /** OverrideMaterials.Num() before the swap: the restore drops the null entries SetMaterial added to reach the slot. */
    UPROPERTY(Transient)
    int32 NumOverrideMaterials = 0;
};

USTRUCT()
struct HARMONYCAPTURE_API FHarmonyMaterialSavedSlots
{
    GENERATED_BODY()

    UPROPERTY(Transient)
    TArray<FHarmonyMaterialSavedSlot> Slots;
};

/** P10: the built-in variant source - options as lists of material swaps, no configuration system needed. */
UCLASS(meta=(DisplayName="Material Swaps"))
class HARMONYCAPTURE_API UHarmonyMaterialVariantSource : public UHarmonyVariantSource
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    TArray<FHarmonyMaterialVariantSet> Sets;

    virtual TArray<FHarmonyCaptureVariantSet> GetVariantSets_Implementation(UObject* WorldContextObject) const override;
    virtual bool ApplyVariant_Implementation(UObject* WorldContextObject, FName SetName, FName OptionName) override;

private:
    /** Per set: the materials the swaps replaced (restored by the base option). Held strongly: a swapped-out material may
        have nothing else referencing it until it is restored. NonTransactional (here and on Current): this is live state
        of meshes whose SetMaterial is not transacted, so undo / redo of an edit to this source must not roll it back
        (Transient alone still goes into undo records). */
    UPROPERTY(Transient, NonTransactional)
    TMap<FName, FHarmonyMaterialSavedSlots> Saved;

    UPROPERTY(Transient, NonTransactional)
    TMap<FName, FName> Current;
};
