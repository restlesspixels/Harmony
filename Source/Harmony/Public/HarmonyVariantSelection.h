#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "HarmonyVariantSelection.generated.h"

class UHarmonyComponent;
class UHarmonyDataAsset;
struct FHarmonySplatVariantSet;

// Variant dropdowns for a Harmony component, kept out of UHarmonyComponent itself.
//
// Each variant set of the component's asset gets one slot: a property with its own enum (Variant 1 .. Variant 32). Variant 1
// is the set's base appearance, Variant N its (N-1)th baked option. In the editor, entries past the set's option count get
// the enum's Hidden metadata, so the dropdown lists only the variants the asset has, and a slot is hidden when its set does
// not exist or has a single option. Enum and property metadata are global (per type, not per instance), so they are refreshed
// for whichever component the details panel is showing, from the slots' EditCondition functions.
//
// The selection itself lives in UHarmonyComponent::ActiveVariants (set through SetVariant), so the Variant Manager, Blueprints
// and undo keep working; the slot values are transient and re-read from it.

UENUM()
enum class EHarmonyVariantSlot1 : uint8
{
    Variant1 UMETA(DisplayName="Variant 1"),
    Variant2 UMETA(DisplayName="Variant 2"),
    Variant3 UMETA(DisplayName="Variant 3"),
    Variant4 UMETA(DisplayName="Variant 4"),
    Variant5 UMETA(DisplayName="Variant 5"),
    Variant6 UMETA(DisplayName="Variant 6"),
    Variant7 UMETA(DisplayName="Variant 7"),
    Variant8 UMETA(DisplayName="Variant 8"),
    Variant9 UMETA(DisplayName="Variant 9"),
    Variant10 UMETA(DisplayName="Variant 10"),
    Variant11 UMETA(DisplayName="Variant 11"),
    Variant12 UMETA(DisplayName="Variant 12"),
    Variant13 UMETA(DisplayName="Variant 13"),
    Variant14 UMETA(DisplayName="Variant 14"),
    Variant15 UMETA(DisplayName="Variant 15"),
    Variant16 UMETA(DisplayName="Variant 16"),
    Variant17 UMETA(DisplayName="Variant 17"),
    Variant18 UMETA(DisplayName="Variant 18"),
    Variant19 UMETA(DisplayName="Variant 19"),
    Variant20 UMETA(DisplayName="Variant 20"),
    Variant21 UMETA(DisplayName="Variant 21"),
    Variant22 UMETA(DisplayName="Variant 22"),
    Variant23 UMETA(DisplayName="Variant 23"),
    Variant24 UMETA(DisplayName="Variant 24"),
    Variant25 UMETA(DisplayName="Variant 25"),
    Variant26 UMETA(DisplayName="Variant 26"),
    Variant27 UMETA(DisplayName="Variant 27"),
    Variant28 UMETA(DisplayName="Variant 28"),
    Variant29 UMETA(DisplayName="Variant 29"),
    Variant30 UMETA(DisplayName="Variant 30"),
    Variant31 UMETA(DisplayName="Variant 31"),
    Variant32 UMETA(DisplayName="Variant 32"),
};

UENUM()
enum class EHarmonyVariantSlot2 : uint8
{
    Variant1 UMETA(DisplayName="Variant 1"),
    Variant2 UMETA(DisplayName="Variant 2"),
    Variant3 UMETA(DisplayName="Variant 3"),
    Variant4 UMETA(DisplayName="Variant 4"),
    Variant5 UMETA(DisplayName="Variant 5"),
    Variant6 UMETA(DisplayName="Variant 6"),
    Variant7 UMETA(DisplayName="Variant 7"),
    Variant8 UMETA(DisplayName="Variant 8"),
    Variant9 UMETA(DisplayName="Variant 9"),
    Variant10 UMETA(DisplayName="Variant 10"),
    Variant11 UMETA(DisplayName="Variant 11"),
    Variant12 UMETA(DisplayName="Variant 12"),
    Variant13 UMETA(DisplayName="Variant 13"),
    Variant14 UMETA(DisplayName="Variant 14"),
    Variant15 UMETA(DisplayName="Variant 15"),
    Variant16 UMETA(DisplayName="Variant 16"),
    Variant17 UMETA(DisplayName="Variant 17"),
    Variant18 UMETA(DisplayName="Variant 18"),
    Variant19 UMETA(DisplayName="Variant 19"),
    Variant20 UMETA(DisplayName="Variant 20"),
    Variant21 UMETA(DisplayName="Variant 21"),
    Variant22 UMETA(DisplayName="Variant 22"),
    Variant23 UMETA(DisplayName="Variant 23"),
    Variant24 UMETA(DisplayName="Variant 24"),
    Variant25 UMETA(DisplayName="Variant 25"),
    Variant26 UMETA(DisplayName="Variant 26"),
    Variant27 UMETA(DisplayName="Variant 27"),
    Variant28 UMETA(DisplayName="Variant 28"),
    Variant29 UMETA(DisplayName="Variant 29"),
    Variant30 UMETA(DisplayName="Variant 30"),
    Variant31 UMETA(DisplayName="Variant 31"),
    Variant32 UMETA(DisplayName="Variant 32"),
};

UENUM()
enum class EHarmonyVariantSlot3 : uint8
{
    Variant1 UMETA(DisplayName="Variant 1"),
    Variant2 UMETA(DisplayName="Variant 2"),
    Variant3 UMETA(DisplayName="Variant 3"),
    Variant4 UMETA(DisplayName="Variant 4"),
    Variant5 UMETA(DisplayName="Variant 5"),
    Variant6 UMETA(DisplayName="Variant 6"),
    Variant7 UMETA(DisplayName="Variant 7"),
    Variant8 UMETA(DisplayName="Variant 8"),
    Variant9 UMETA(DisplayName="Variant 9"),
    Variant10 UMETA(DisplayName="Variant 10"),
    Variant11 UMETA(DisplayName="Variant 11"),
    Variant12 UMETA(DisplayName="Variant 12"),
    Variant13 UMETA(DisplayName="Variant 13"),
    Variant14 UMETA(DisplayName="Variant 14"),
    Variant15 UMETA(DisplayName="Variant 15"),
    Variant16 UMETA(DisplayName="Variant 16"),
    Variant17 UMETA(DisplayName="Variant 17"),
    Variant18 UMETA(DisplayName="Variant 18"),
    Variant19 UMETA(DisplayName="Variant 19"),
    Variant20 UMETA(DisplayName="Variant 20"),
    Variant21 UMETA(DisplayName="Variant 21"),
    Variant22 UMETA(DisplayName="Variant 22"),
    Variant23 UMETA(DisplayName="Variant 23"),
    Variant24 UMETA(DisplayName="Variant 24"),
    Variant25 UMETA(DisplayName="Variant 25"),
    Variant26 UMETA(DisplayName="Variant 26"),
    Variant27 UMETA(DisplayName="Variant 27"),
    Variant28 UMETA(DisplayName="Variant 28"),
    Variant29 UMETA(DisplayName="Variant 29"),
    Variant30 UMETA(DisplayName="Variant 30"),
    Variant31 UMETA(DisplayName="Variant 31"),
    Variant32 UMETA(DisplayName="Variant 32"),
};

UENUM()
enum class EHarmonyVariantSlot4 : uint8
{
    Variant1 UMETA(DisplayName="Variant 1"),
    Variant2 UMETA(DisplayName="Variant 2"),
    Variant3 UMETA(DisplayName="Variant 3"),
    Variant4 UMETA(DisplayName="Variant 4"),
    Variant5 UMETA(DisplayName="Variant 5"),
    Variant6 UMETA(DisplayName="Variant 6"),
    Variant7 UMETA(DisplayName="Variant 7"),
    Variant8 UMETA(DisplayName="Variant 8"),
    Variant9 UMETA(DisplayName="Variant 9"),
    Variant10 UMETA(DisplayName="Variant 10"),
    Variant11 UMETA(DisplayName="Variant 11"),
    Variant12 UMETA(DisplayName="Variant 12"),
    Variant13 UMETA(DisplayName="Variant 13"),
    Variant14 UMETA(DisplayName="Variant 14"),
    Variant15 UMETA(DisplayName="Variant 15"),
    Variant16 UMETA(DisplayName="Variant 16"),
    Variant17 UMETA(DisplayName="Variant 17"),
    Variant18 UMETA(DisplayName="Variant 18"),
    Variant19 UMETA(DisplayName="Variant 19"),
    Variant20 UMETA(DisplayName="Variant 20"),
    Variant21 UMETA(DisplayName="Variant 21"),
    Variant22 UMETA(DisplayName="Variant 22"),
    Variant23 UMETA(DisplayName="Variant 23"),
    Variant24 UMETA(DisplayName="Variant 24"),
    Variant25 UMETA(DisplayName="Variant 25"),
    Variant26 UMETA(DisplayName="Variant 26"),
    Variant27 UMETA(DisplayName="Variant 27"),
    Variant28 UMETA(DisplayName="Variant 28"),
    Variant29 UMETA(DisplayName="Variant 29"),
    Variant30 UMETA(DisplayName="Variant 30"),
    Variant31 UMETA(DisplayName="Variant 31"),
    Variant32 UMETA(DisplayName="Variant 32"),
};
UCLASS(DefaultToInstanced, EditInlineNew, CollapseCategories, BlueprintType)
class HARMONY_API UHarmonyVariantSelection : public UObject
{
    GENERATED_BODY()

public:
    static constexpr int32 MaxSets = 4;
    static constexpr int32 MaxOptionsPerSet = 32;

    UPROPERTY(EditAnywhere, Transient, Category="Variants", meta=(DisplayName="Variant", EditCondition="IsSlot1Shown()", EditConditionHides, NoResetToDefault))
    EHarmonyVariantSlot1 Slot1 = EHarmonyVariantSlot1::Variant1;

    UPROPERTY(EditAnywhere, Transient, Category="Variants", meta=(DisplayName="Variant Set 2", EditCondition="IsSlot2Shown()", EditConditionHides, NoResetToDefault))
    EHarmonyVariantSlot2 Slot2 = EHarmonyVariantSlot2::Variant1;

    UPROPERTY(EditAnywhere, Transient, Category="Variants", meta=(DisplayName="Variant Set 3", EditCondition="IsSlot3Shown()", EditConditionHides, NoResetToDefault))
    EHarmonyVariantSlot3 Slot3 = EHarmonyVariantSlot3::Variant1;

    UPROPERTY(EditAnywhere, Transient, Category="Variants", meta=(DisplayName="Variant Set 4", EditCondition="IsSlot4Shown()", EditConditionHides, NoResetToDefault))
    EHarmonyVariantSlot4 Slot4 = EHarmonyVariantSlot4::Variant1;

    // Seconds a switch between variants takes: the splats fade from what is shown to the new variant (colour, opacity,
    // position and shape). 0 switches at once. Applies to every way of switching (these dropdowns, SetVariant, the Variant
    // Manager); UHarmonyComponent::SetVariantWithDuration overrides it for one switch.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants", meta=(DisplayName="Transition Duration", ClampMin="0.0", UIMax="5.0", Units="s", EditCondition="IsTransitionShown()", EditConditionHides, HideEditConditionToggle))
    float TransitionDuration = 0.5f;

    // Number of variant sets of the asset that have a dropdown (sets past MaxSets are not offered).
    UFUNCTION(BlueprintPure, Category="Gaussian Splat|Variants")
    int32 GetNumVariantSets() const;

    // Number of variants of set SetIndex, base included (0 when there is no such set).
    UFUNCTION(BlueprintPure, Category="Gaussian Splat|Variants")
    int32 GetNumVariants(int32 SetIndex) const;

    // The variant set SetIndex shows, 0-based (0: the base appearance).
    UFUNCTION(BlueprintPure, Category="Gaussian Splat|Variants")
    int32 GetSelectedVariant(int32 SetIndex) const;

    // Shows variant VariantIndex (0-based, 0: the base appearance) of set SetIndex. Returns false when either is out of range.
    UFUNCTION(BlueprintCallable, Category="Gaussian Splat|Variants")
    bool SelectVariant(int32 SetIndex, int32 VariantIndex);

    // True when at least one set has two or more variants (the component shows this object only then).
    bool HasChoices() const;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
    UFUNCTION()
    bool IsTransitionShown() const { return HasChoices(); }
    UFUNCTION()
    bool IsSlot1Shown() const { return IsSlotShown(0); }
    UFUNCTION()
    bool IsSlot2Shown() const { return IsSlotShown(1); }
    UFUNCTION()
    bool IsSlot3Shown() const { return IsSlotShown(2); }
    UFUNCTION()
    bool IsSlot4Shown() const { return IsSlotShown(3); }

    bool IsSlotShown(int32 SetIndex) const;

    UHarmonyComponent* GetComponent() const;
    const FHarmonySplatVariantSet* GetSet(int32 SetIndex) const;

    // Variant N of a set (N >= 1) is its Nth option other than the base option.
    static TArray<FName> GetVariantOptionNames(const FHarmonySplatVariantSet& Set);

    uint8& SlotValue(int32 SetIndex);
    static FName SlotPropertyName(int32 SetIndex);

#if WITH_EDITOR
    // Re-reads the slot values from the component and updates the slot enums' Hidden metadata (only when a count changed).
    void RefreshEditorDisplay() const;
    static UEnum* SlotEnum(int32 SetIndex);
#endif
};
