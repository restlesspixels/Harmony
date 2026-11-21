#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"

#include "HarmonyComponent.generated.h"

class UHarmonyDataAsset;
class UHarmonyVariantSelection;

UENUM(BlueprintType)
enum class EHarmonySplatDepthOffsetMode : uint8
{
    SimpleViewSpace UMETA(DisplayName="Simple View-Space Offset"),
    GroundWeighted UMETA(DisplayName="Ground-Weighted Offset")
};

USTRUCT(BlueprintType)
struct HARMONY_API FHarmonyAlphaFadeSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category="Near Fade", meta=(DisplayName="Camera Near Fade", ClampMin="0.0", UIMin="0.0", UIMax="5000.0"))
    float CameraNearFade = 100.0f;

    UPROPERTY(EditAnywhere, Category="Near Fade", meta=(DisplayName="Camera Near Fade Feather", ClampMin="0.0", UIMin="0.0", UIMax="5000.0"))
    float CameraNearFadeFeather = 200.0f;

    UPROPERTY(EditAnywhere, Category="Near Fade", meta=(DisplayName="Ground Alpha Fade Position", UIMin="-100000.0", UIMax="100000.0"))
    float GroundAlphaFadePosition = 0.0f;

    UPROPERTY(EditAnywhere, Category="Near Fade", meta=(DisplayName="Ground Alpha Fade Feather", ClampMin="0.0", UIMin="0.0", UIMax="5000.0"))
    float GroundAlphaFadeFeather = 0.0f;
};

USTRUCT(BlueprintType)
struct HARMONY_API FHarmonyDepthOffsetSettings
{
    GENERATED_BODY()

    // Selects how SplatDepthOffset is applied.
    UPROPERTY(EditAnywhere, Category="Depth Offset", meta=(DisplayName="Mode"))
    EHarmonySplatDepthOffsetMode SplatDepthOffsetMode = EHarmonySplatDepthOffsetMode::GroundWeighted;

    // Offsets this Harmony actor in view-space depth.
    // Positive values push splats farther away; negative values pull splats closer.
    // In Ground-Weighted mode this is the maximum offset applied near the ground, fading out with height.
    UPROPERTY(EditAnywhere, Category="Depth Offset", meta=(DisplayName="Splat Depth Offset", UIMin="-500.0", UIMax="500.0"))
    float SplatDepthOffset = 200.0f;

    // Local-space Z reference where Ground-Weighted depth offset is applied at full strength.
    UPROPERTY(EditAnywhere, Category="Depth Offset", meta=(DisplayName="Ground Offset Base Local Z", EditCondition="SplatDepthOffsetMode == EHarmonySplatDepthOffsetMode::GroundWeighted", EditConditionHides, UIMin="-100000.0", UIMax="100000.0"))
    float GroundDepthOffsetBaseLocalZ = 40.0f;

    // World-space height range over which Ground-Weighted depth offset fades to zero above Ground Offset Base Local Z.
    UPROPERTY(EditAnywhere, Category="Depth Offset", meta=(DisplayName="Ground Offset Fade Height", EditCondition="SplatDepthOffsetMode == EHarmonySplatDepthOffsetMode::GroundWeighted", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="2000.0"))
    float GroundDepthOffsetFadeHeight = 60.0f;
};

UCLASS(ClassGroup=(Rendering), meta=(BlueprintSpawnableComponent), showcategories=(Transform), hidecategories=(Collision, Physics, Networking, HLOD, Cooking, DataLayers, AssetUserData, Activation, Navigation, Tags, Replication, Input, LOD))
class HARMONY_API UHarmonyComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UHarmonyComponent();

    UPROPERTY(EditAnywhere, Category="Gaussian Splat")
    TSoftObjectPtr<UHarmonyDataAsset> SplatDataAsset;

    // Multiplies the final splat alpha for this Harmony actor after preprocess opacity is evaluated.
    // Values above 1 make the actor denser; values below 1 make it more transparent.
    UPROPERTY(EditAnywhere, Category="Gaussian Splat", meta=(ClampMin="0.0", UIMin="0.0", UIMax="4.0"))
    float OpacityScale = 1.0f;

    // Multiplies the projected splat size for this Harmony actor.
    // Values above 1 make splats larger on screen; values below 1 make them smaller.
    UPROPERTY(EditAnywhere, Category="Gaussian Splat", meta=(ClampMin="0.0", UIMin="0.0", UIMax="4.0"))
    float SplatScale = 1.0f;

    UPROPERTY(EditAnywhere, Category="Gaussian Splat", meta=(DisplayName="Near Fade"))
    FHarmonyAlphaFadeSettings AlphaFadeSettings;

    UPROPERTY(EditAnywhere, Category="Gaussian Splat", meta=(DisplayName="Depth Offset"))
    FHarmonyDepthOffsetSettings DepthOffsetSettings;

    // Allows this component's background splats to contribute to Harmony's background depth estimate,
    // SceneDepth, and the matching camera-relative scene velocity used for temporal reprojection.
    // This currently affects the main splat layer.
    UPROPERTY(EditAnywhere, Category="Gaussian Splat", meta=(DisplayName="Write Depth To Scene", ToolTip="Write this component's representative splat depth after standard translucency, before depth-driven post effects such as DOF and TSR."))
    bool bWriteDepthToScene = true;

    // P10 configurable variants: the option shown per variant set of the asset (set name -> option name). A set that is not
    // listed, or lists its base option or an unknown option, shows the base splats. Edit here, call SetVariant (Blueprint,
    // or a Variant Manager Function Caller), or let a variant source drive it.
    // Not shown in the details panel: the Variants dropdowns (VariantSelection) edit it.
    UPROPERTY(BlueprintReadOnly, Category="Gaussian Splat|Variants")
    TMap<FName, FName> ActiveVariants;

    // Variant dropdowns (one per variant set of the asset; all logic in UHarmonyVariantSelection). The details panel already
    // lists this instanced object's properties in their own "Variants" category, so this row itself is always hidden (it
    // would show the same dropdowns a second time).
    UPROPERTY(VisibleAnywhere, Instanced, BlueprintReadOnly, Category="Gaussian Splat|Variants", meta=(DisplayName="Variants", EditCondition="false", EditConditionHides))
    TObjectPtr<UHarmonyVariantSelection> VariantSelection;

    // Shows Option of variant set SetName (NAME_None or the set's base option: back to the base splats). Returns false when
    // the asset has no such set or option (the selection is still stored, so it applies once a matching asset is set).
    UFUNCTION(BlueprintCallable, Category="Gaussian Splat|Variants")
    bool SetVariant(FName SetName, FName OptionName);

    // SetVariant with its own transition time for this switch (seconds; 0: at once) instead of the Variants section's
    // Transition Duration.
    UFUNCTION(BlueprintCallable, Category="Gaussian Splat|Variants")
    bool SetVariantWithDuration(FName SetName, FName OptionName, float DurationSeconds);

    // True while the renderer is fading this component from one variant to another (or is about to).
    UFUNCTION(BlueprintPure, Category="Gaussian Splat|Variants")
    bool IsVariantTransitionActive() const { return bVariantTransitionActive; }

    // Renderer: the transition time for the variant change it has just seen (a SetVariantWithDuration value is used once).
    float TakeVariantTransitionDuration() const;
    // RequestedLookWeight: the eased weight (0..1) of the look ActiveVariants asks for (1 when no transition runs).
    void SetVariantTransitionState(bool bActive, float RequestedLookWeight) const;
    float GetVariantTransitionWeight() const { return VariantTransitionWeight; }

    // Called (game thread) whenever the renderer has published the transition state: every frame the component is drawn.
    // UHarmonyLightingComponent follows the splat fade with it, also in editor viewports that do not tick the world.
    DECLARE_MULTICAST_DELEGATE(FOnVariantTransitionUpdated);
    mutable FOnVariantTransitionUpdated OnVariantTransitionUpdated;

    // Every set back to its base option.
    UFUNCTION(BlueprintCallable, Category="Gaussian Splat|Variants")
    void ClearVariants();

    // The asset's variant sets and, per set, its options (base option first).
    UFUNCTION(BlueprintCallable, Category="Gaussian Splat|Variants")
    TArray<FName> GetVariantSetNames() const;

    UFUNCTION(BlueprintCallable, Category="Gaussian Splat|Variants")
    TArray<FName> GetVariantOptionNames(FName SetName) const;

    // Changes with ActiveVariants. The renderer does not use it: it hashes the options that resolve on the asset, so entries
    // for sets the asset does not have never force a re-upload.
    uint32 GetVariantStateHash() const;

    // SplatDataAsset, loaded if the renderer has not loaded it yet.
    const UHarmonyDataAsset* ResolveVariantAsset() const;

    // SplatDataAsset, loaded synchronously if it is not loaded, and kept referenced by this component (the soft pointer alone
    // does not keep it alive: in -game and packaged builds every GC would unload it and the next frame reload it).
    UHarmonyDataAsset* ResolveSplatDataAsset() const;

    virtual void OnRegister() override;
    virtual void OnUnregister() override;
#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    static void GetComponentsForWorld(const UWorld* World, TArray<UHarmonyComponent*>& OutComponents);

    bool GetEnabled() const { return bEnabled; }
    void SetEnabled(bool bInEnabled);

private:
    UPROPERTY()
    bool bEnabled = true;

    // Strong reference to the asset ResolveSplatDataAsset last resolved (follows SplatDataAsset; cleared on unregister).
    // RF_Standalone keeps loaded assets alive only in the editor. NonTransactional: a cache, not undo state (the undo buffer
    // would otherwise hold the asset too).
    UPROPERTY(Transient, NonTransactional)
    mutable TObjectPtr<UHarmonyDataAsset> ResolvedSplatDataAsset;

    // SetVariantWithDuration's time, until the renderer has used it (negative: none).
    mutable float PendingVariantTransitionDuration = -1.0f;
    mutable bool bVariantTransitionActive = false;
    mutable float VariantTransitionWeight = 1.0f;
};
