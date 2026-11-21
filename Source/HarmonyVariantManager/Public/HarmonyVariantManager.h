// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Containers/Ticker.h"
#include "HarmonyVariantSource.h"
#include "HarmonyVariantManager.generated.h"

class ALevelVariantSetsActor;
class UHarmonyComponent;
class ULevelVariantSets;
class UPropertyValue;
class UVariant;

/** What applying a Level Variant Sets option replaced (UHarmonyLevelVariantSetsSource restores it). */
USTRUCT()
struct FHarmonyLevelVariantSetsSaved
{
    GENERATED_BODY()

    /** Transient copies of the captured properties the option and its enabled dependencies apply, each holding the value
        the scene had before (in the order the option applies them). */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UPropertyValue>> Values;

    /** The objects those values reference: a recorded object pointer is kept in a byte array, which GC does not see. */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UObject>> Objects;
};

/**
 * P10: Unreal's Variant Manager as a capture rig's variant source. Variant sets and variants are matched by their display
 * names: a Variant Manager set "Paint" with variants "Yellow", "Red" bakes Harmony variant set "Paint" with options
 * "Yellow" (the base, if active at capture time) and "Red". Variants that capture no property (Function Callers only)
 * are not offered: the Variant Manager sync cannot tell when they are switched on.
 */
UCLASS(meta=(DisplayName="Level Variant Sets"))
class HARMONYVARIANTMANAGER_API UHarmonyLevelVariantSetsSource : public UHarmonyVariantSource
{
    GENERATED_BODY()

public:
    /** The Level Variant Sets actor in the capture world. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    TSoftObjectPtr<ALevelVariantSetsActor> LevelVariantSetsActor;

    virtual TArray<FHarmonyCaptureVariantSet> GetVariantSets_Implementation(UObject* WorldContextObject) const override;
    virtual bool ApplyVariant_Implementation(UObject* WorldContextObject, FName SetName, FName OptionName) override;

private:
    /** Records the current values of everything Variant (and its enabled dependencies) is about to apply, into Saved[SetName]. */
    void SaveValues(FName SetName, UVariant* Variant);

    /** Puts back the values Saved[SetName] holds (newest first) and forgets them. */
    void RestoreValues(FName SetName);

    /** Per set: what the option applied through this source replaced. Switching the base variant on again only re-applies
        what the base captures, so properties only the option (or a dependency, possibly in another set) changed would stay
        changed without this. NonTransactional: live scene state, undo / redo of an edit to this source must not roll it
        back (Transient alone still goes into undo records). DuplicateTransient: a duplicated rig applied nothing. */
    UPROPERTY(Transient, DuplicateTransient, NonTransactional)
    TMap<FName, FHarmonyLevelVariantSetsSaved> Saved;
};

/**
 * P10: keeps a Harmony actor's splat variants in step with the Variant Manager. Add it to the actor that has the
 * UHarmonyComponent and point it at the Level Variant Sets actor; whenever a variant is switched on (Variant Manager UI,
 * Blueprint, runtime UI), each Harmony variant set shows the Variant Manager variant of the same name that is active.
 * No Function Callers needed. Works in the editor and at runtime.
 * Switched off (Auto Activate unticked, Deactivate / Set Active false) it leaves the Harmony component alone, so its
 * variants can be set by hand; switched back on it applies the Variant Manager's state once.
 */
UCLASS(ClassGroup=(Rendering), meta=(BlueprintSpawnableComponent, DisplayName="Harmony Variant Manager Sync"))
class HARMONYVARIANTMANAGER_API UHarmonyVariantManagerSync : public UActorComponent
{
    GENERATED_BODY()

public:
    UHarmonyVariantManagerSync();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    TSoftObjectPtr<ALevelVariantSetsActor> LevelVariantSetsActor;

    /** The Harmony component to drive (empty: the owner's first one). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants", meta=(UseComponentPicker, AllowedClasses="/Script/Harmony.HarmonyComponent"))
    FComponentReference HarmonyComponent;

    /** Reads the Variant Manager's active variants and applies them to the Harmony component now. */
    UFUNCTION(BlueprintCallable, CallInEditor, Category="Variants")
    void SyncNow();

    virtual void OnRegister() override;
    virtual void OnUnregister() override;
    virtual void Activate(bool bReset = false) override;
#if WITH_EDITOR
    /** Auto Activate is the editor's on / off switch: ticking or unticking it switches the sync at once. */
    virtual void PostEditChangeProperty(FPropertyChangedEvent& Event) override;
#endif

private:
    /** Core ticker (runs in non-realtime editor viewports and while the game is paused): a sync right after a variant's
        properties are applied, and a low-rate poll that also catches Switch Actor captures (applied without
        OnPropertyApplied) and rebinds when the Level Variant Sets resolve late or change. */
    bool Poll(float DeltaTime);
    void Bind(ULevelVariantSets* Sets, int32 NumProperties);
    void Unbind();
    void OnPropertyApplied();
    UHarmonyComponent* ResolveHarmony() const;
    bool IsSuspended() const;

    TArray<TPair<TWeakObjectPtr<UPropertyValue>, FDelegateHandle>> Bindings;

    /** Held strongly, with the assets the bound variants record (materials, meshes): in -game and packaged builds nothing
        else may hold them (the actor's asset reference is a soft path, a recorded object a byte copy), so every GC would
        drop them and the next poll reload them synchronously. DuplicateTransient: a duplicate (PIE) binds itself. */
    UPROPERTY(Transient, DuplicateTransient, NonTransactional)
    TObjectPtr<ULevelVariantSets> BoundSets;

    UPROPERTY(Transient, DuplicateTransient, NonTransactional)
    TArray<TObjectPtr<UObject>> HeldRecordedObjects;

    int32 BoundProperties = -1;
    FTSTicker::FDelegateHandle TickerHandle;
    double NextPoll = 0.0;
    bool bSyncPending = false;
    TMap<FName, FName> LastApplied;
};
