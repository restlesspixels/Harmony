#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "HarmonyLightingComponent.generated.h"

class UCurveFloat;
class UDirectionalLightComponent;
class UPostProcessComponent;
class UHarmonyComponent;
class USkyLightComponent;
struct FHarmonyLightingProbe;
class UTextureCube;

/** When UHarmonyLightingComponent lights the level. */
UENUM(BlueprintType)
enum class EHarmonyLightScene : uint8
{
    /** Light the level with the asset's lighting probe, unless the level has a sky light of its own (the engine uses one
        sky light per world: the level's stays in charge). */
    Auto,
    /** Light the level with the probe even if the level has its own sky light (the Harmony one takes over). */
    Always,
    /** Never. */
    Off
};

/** The sun of the lighting probe in use, as UHarmonyLightingComponent shows it (blended during a variant fade). */
struct FHarmonySunState
{
    bool bValid = false;                                // the probe stores a sun
    FVector Direction = FVector::UpVector;              // towards the sun (world)
    FLinearColor Color = FLinearColor::White;           // at the ground, largest channel 1
    float Illuminance = 0.0f;                           // lux at the ground
    FLinearColor SunFraction = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);   // the sun's share of the ground's light; 0 when it does not reach the stand
    // the probe itself (set whether it stores a sun or not)
    float RadianceScale = 1.0f;                         // its cubemap's texels x this = scene radiance
    bool bHasExposure = false;                          // the exposure its capture (and so the splats) was rendered at is known
    float ExposureEV100 = 0.0f;
};

/**
 * Phase 13: lights the level with the lighting probe of the owner's Harmony asset (UHarmonyDataAsset::LightingProbe, and
 * the one of the active variant option when it has its own), through a sky light it creates at run time. The splats are
 * already lit; this is for what is put into the environment: a vehicle gets the lighting the scene was captured in.
 *
 * Kept out of UHarmonyComponent on purpose. No probe in the asset: no sky light, no effect. On a variant switch the
 * lighting goes from the old probe to the new one with the splats' fade: mixed by the fade's own weight where the
 * renderer mixes a sky light's two cubemaps, and changed over at the middle of the fade under Lumen, which does not
 * (Lumen's accumulation over the next frames softens it). An option without a probe of its own keeps the asset's.
 */
UCLASS(ClassGroup=(Harmony), meta=(BlueprintSpawnableComponent))
class HARMONY_API UHarmonyLightingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHarmonyLightingComponent();

    /** Whether the asset's lighting probe lights the level. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lighting")
    EHarmonyLightScene LightScene = EHarmonyLightScene::Auto;

    /** Phase 14: whether a directional light made from the probe's stored sun lights the level too (the sky light holds
        the sky and the surroundings, not the sun). Auto: unless the level has a directional light of its own, or the
        capture found the stand out of the sun (indoors, in shade). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lighting")
    EHarmonyLightScene SunLight = EHarmonyLightScene::Auto;

    /** Whether the Harmony actor also sets the level's exposure to the one the capture was rendered at (an unbound
        post-process component of the lowest priority), so that what the probe lights is as bright on screen as the splats
        are, whatever the level's auto exposure would do. Auto: while the probe lights the level, unless the level has an
        exposure setup of its own (an enabled unbound post-process volume that overrides exposure). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lighting")
    EHarmonyLightScene Exposure = EHarmonyLightScene::Auto;

    /** Scales the probe's light (1: the captured scene's own brightness). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lighting", meta=(ClampMin="0.0", UIMax="4.0"))
    float IntensityScale = 1.0f;

    /** The probe in use (the asset's, or the active variant option's): empty when the asset has none. */
    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Lighting")
    TObjectPtr<UTextureCube> CurrentProbe;

    /** The exposure (EV100) the capture was rendered at: lit meshes match the splats' brightness at this exposure. */
    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Lighting")
    float CaptureEV100 = 0.0f;

    /** Whether the probe lights the level right now (it has one, it is allowed, and the level's own sky light does not
        take precedence). */
    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Lighting")
    bool bLightingActive = false;

    /** Whether the Harmony actor sets the level's exposure right now (Exposure), and the EV100 it is at. After a variant
        switch it moves to the new look's exposure over a few seconds, the lights compensating meanwhile. */
    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Lighting")
    bool bExposureActive = false;

    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Lighting")
    float AppliedEV100 = 0.0f;

    /** Why not, when it does not ("the asset has no lighting probe", "the level has its own sky light", ...). */
    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Lighting")
    FString LightingStatus;

    UFUNCTION(BlueprintCallable, Category="Lighting")
    void SetLightScene(EHarmonyLightScene InLightScene);

    /** Brings the sky light in line with the asset, the active variant and the splat fade. Runs every tick and every time
        the renderer draws the splats; call it after changing things from code if you cannot wait for either. */
    UFUNCTION(BlueprintCallable, Category="Lighting")
    void UpdateLighting();

    /** The probe's sun as it is shown now. False: the asset has no probe. */
    bool GetSunState(FHarmonySunState& Out) const;

    /** Phase 16: the probe(s) the splats show now, as the splats' fade mixes them: From alone at Fraction 0, To alone at 1
        (To null: From alone); texel x scale = scene radiance. Without a sky light of its own (the level has one, Light
        Scene Off): the target probe alone. False: the asset has no probe. */
    bool GetProbeBlend(UTextureCube*& OutFrom, float& OutFromScale, UTextureCube*& OutTo, float& OutToScale, float& OutFraction) const;

    virtual void OnRegister() override;
    virtual void OnUnregister() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
    UHarmonyComponent* FindHarmonyComponent() const;
    void HandleVariantTransitionUpdated();
    bool LevelHasOwnSkyLight() const;
    void EnsureSkyLight(int32 Resolution);
    void DestroySkyLight();
    bool LevelHasOwnSun() const;
    FHarmonySunState MakeSunState(const FHarmonyLightingProbe* Probe) const;
    void UpdateSunLight(bool bAllowed);
    void DestroySunLight();
    bool LevelHasOwnExposure() const;
    void UpdateExposure(bool bWanted, float TargetEV100);
    void DestroyExposure();

    UPROPERTY(Transient)
    TObjectPtr<USkyLightComponent> SkyLight;

    UPROPERTY(Transient)
    TObjectPtr<UDirectionalLightComponent> SunLightComponent;

    UPROPERTY(Transient)
    TObjectPtr<UPostProcessComponent> ExposureComponent;

    UPROPERTY(Transient)
    TObjectPtr<UCurveFloat> FlatExposureCurve;

    double ExposureTime = 0.0;
    bool bLevelHasOwnExposure = false;
    uint32 ShownStateHash = 0u;     // what is on screen: when it changes, editor viewports that are not realtime are redrawn

    // The suns of BlendFrom and BlendTo, and what is shown (theirs mixed by the blend's fraction).
    FHarmonySunState FromSun;
    FHarmonySunState ToSun;
    FHarmonySunState CurrentSun;
    bool bHasCurrentSun = false;

    // The blend the sky light shows: BlendFrom alone at fraction 0, BlendTo alone at 1 (BlendTo null: BlendFrom alone).
    UPROPERTY(Transient)
    TObjectPtr<UTextureCube> BlendFrom;
    UPROPERTY(Transient)
    TObjectPtr<UTextureCube> BlendTo;
    float BlendFraction = 0.0f;
    // A cubemap that was just put into the sky light is not filtered yet: until this frame the other one shows alone.
    uint64 HoldFractionUntilFrame = 0u;
    float HeldFraction = 0.0f;
    // The renderer's last call (it drives the blend while the splats are drawn).
    uint64 RendererUpdateFrame = 0u;
    bool bInRendererUpdate = false;

    TWeakObjectPtr<UHarmonyComponent> BoundComponent;
    FDelegateHandle TransitionHandle;
    uint64 LevelSkyLightCheckedFrame = 0u;
    bool bLevelHasOwnSkyLight = false;
    bool bLevelHasOwnSun = false;
};
