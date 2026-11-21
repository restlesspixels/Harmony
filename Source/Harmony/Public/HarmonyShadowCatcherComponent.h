#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "HarmonyShadowCatcherComponent.generated.h"

class FSceneView;
class FSceneViewFamily;
class UHarmonyComponent;
class UHarmonyLightingComponent;
class USceneCaptureComponent2D;
class UTexture2D;
class UTextureRenderTarget2D;

/** What the shadows of UHarmonyShadowCatcherComponent fall on. */
UENUM(BlueprintType)
enum class EHarmonyShadowGround : uint8
{
    /** The asset's shadow proxy (a heightfield of the ground, taken at capture) when it has one, else a plane. */
    Auto,
    /** A horizontal plane at Plane Height. */
    Plane
};

/**
 * Phase 14: the splats catch the shadows of what is put into the environment (a vehicle). The splats are an image: no
 * engine light can darken them. Harmony draws the shadow itself, in a pass right after the splats are composed into the
 * scene, from two small depth captures of the caster actors: one along the sun (the sun's shadow) and one from below
 * (how much of the sky the caster hides from the ground under and around it: the contact shadow, sharp at the tyres and
 * soft under the body). The ground is the asset's shadow proxy or a plane.
 *
 *   pixel' = T(k * Tinv(pixel)),  k = f * Vsun + (1 - f) * Vsky
 *
 * f: the sun's share of the ground's light (measured at capture, with the lighting probe; 0 when the sun does not reach
 * the stand), Vsun / Vsky: how much of the sun / the sky the casters leave visible, T: the tone curve.
 *
 * No casters, the component off, or no Harmony asset: no captures, no textures, no pass.
 *
 * Phase 16 (Catch Reflections): on a shiny floor the casters are also reflected in it, as strongly and as sharply as the
 * floor's material allows (the asset's reflection proxy: roughness, specular and metallic around the stand, baked at
 * capture with the rig's Floor Reflection). For each view the casters are rendered mirrored about the floor's plane (a
 * scene capture of the casters alone); the pass blurs that by the roughness, weights it by the specular x Fresnel and adds
 * it where the floor is, and dims the floor's own baked reflection (the lighting probe in the mirror direction) where the
 * casters' reflection covers it. An asset without a reflection proxy: nothing (no capture, no pass).
 */
UCLASS(ClassGroup=(Harmony), meta=(BlueprintSpawnableComponent))
class HARMONY_API UHarmonyShadowCatcherComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHarmonyShadowCatcherComponent();

    /** Draw the casters' shadows on the splats. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shadow Catcher")
    bool bCatchShadows = true;

    /** The actors whose shadows the splats catch (the vehicle). */
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Shadow Catcher")
    TArray<TObjectPtr<AActor>> Casters;

    /** Every actor of the level with this tag is a caster too. In the editor the level is searched again when an actor's
        tags are edited, an actor is added or deleted, or a level is loaded; in a game once at Begin Play and when a level
        streams in: call Refresh Casters after tagging or spawning a caster at run time. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shadow Catcher")
    FName CasterTag = TEXT("HarmonyShadowCaster");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shadow Catcher")
    EHarmonyShadowGround Ground = EHarmonyShadowGround::Auto;

    /** Height (cm, in the Harmony actor's space) of the ground plane, when a plane is used. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shadow Catcher")
    float PlaneHeight = 0.0f;

    /** Strength of the contact shadow (the sky the caster hides). 0: none. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shadow Catcher", meta=(ClampMin="0.0", ClampMax="1.0"))
    float ContactStrength = 1.0f;

    /** How far the contact shadow spreads for the height of what casts it (1: a 45 degree cone). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shadow Catcher", AdvancedDisplay, meta=(ClampMin="0.1", ClampMax="4.0"))
    float ContactSpread = 1.5f;

    /** Strength of the sun's shadow. 0: none. It also needs a lighting probe that says the sun reaches the stand. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shadow Catcher", meta=(ClampMin="0.0", ClampMax="1.0"))
    float SunStrength = 1.0f;

    /** Angular size of the sun (degrees): the softness of its shadow's edge away from the caster. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shadow Catcher", AdvancedDisplay, meta=(ClampMin="0.1", ClampMax="10.0"))
    float SunAngle = 1.0f;

    /** Resolution of the two caster depth captures. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shadow Catcher", AdvancedDisplay, meta=(ClampMin="64", ClampMax="2048"))
    int32 CasterMapResolution = 512;

    /** Whether shadows are drawn right now, and why not. */
    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Shadow Catcher")
    FString ShadowStatus;

    /** Phase 16: reflect the casters in the floor where it is shiny. Needs an asset captured with a reflection proxy (the
        capture rig's Floor Reflection: Auto bakes one only for a floor that shows a reflection). Off everywhere:
        r.Harmony.Reflections 0. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reflections")
    bool bCatchReflections = true;

    /** Strength of the reflection (1: as much as the floor's material reflects). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reflections", meta=(ClampMin="0.0", ClampMax="2.0"))
    float ReflectionStrength = 1.0f;

    /** Resolution of the mirrored capture of the casters, against the view's. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reflections", AdvancedDisplay, meta=(ClampMin="0.25", ClampMax="1.0"))
    float ReflectionResolutionScale = 0.5f;

    /** Phase 16: a hidden floor under the casters that Lumen sees (it occludes and bounces light; nothing draws it): the
        vehicle's underside and lower body are lit by a floor the vehicle shadows, as in the real scene, instead of the
        probe's bright, unshadowed floor. In the view and in the reflection's mirror. Placed at the asset's reflection
        proxy plane (else the shadow proxy's height at the stand, else Plane Height), in the captured floor colour. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reflections")
    bool bLumenFloor = true;

    /** How far (cm) the hidden floor reaches beyond the casters' footprint. Only the floor under and right beside the
        vehicle is darkened by it in the real scene; further out the probe's own floor lights it (measured 13 Nov: a 24 m
        floor made the car's sides 35 % too dark). Negative: the whole 24 m square of the reflection proxy. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reflections", AdvancedDisplay)
    float LumenFloorMargin = 50.0f;

    /** The hidden floor's colour when the asset's capture did not measure the floor's (older captures). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reflections", AdvancedDisplay)
    FLinearColor LumenFloorColor = FLinearColor(0.6f, 0.6f, 0.6f, 1.0f);

    /** Whether reflections are drawn right now, and why not. */
    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="Reflections")
    FString ReflectionStatus;

    UFUNCTION(BlueprintCallable, Category="Reflections")
    void SetCatchReflections(bool bInCatchReflections);

    /** (the renderer) Renders the casters mirrored about the floor for this view; called from the view's SetupView. */
    void RenderReflectionMirror(const FSceneViewFamily& ViewFamily, const FSceneView& View);

    UFUNCTION(BlueprintCallable, Category="Shadow Catcher")
    void SetCatchShadows(bool bInCatchShadows);

    /** Searches the level for actors with the Caster Tag again (the Casters list needs no refresh). */
    UFUNCTION(BlueprintCallable, Category="Shadow Catcher")
    void RefreshCasters();

    /** Brings the captures and what the renderer draws in line with the casters and the sun. Runs every tick. */
    UFUNCTION(BlueprintCallable, Category="Shadow Catcher")
    void UpdateShadows();

    virtual void OnRegister() override;
    virtual void OnUnregister() override;
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
    void GatherCasters(TArray<AActor*>& Out);
    void Release();
    USceneCaptureComponent2D* EnsureCapture(TObjectPtr<USceneCaptureComponent2D>& Capture, TObjectPtr<UTextureRenderTarget2D>& Target, const TCHAR* Name);
    void EnsureHeightTexture(const class UHarmonyDataAsset* Asset);
    void UpdateReflections(const class UHarmonyDataAsset* Asset, const TArray<AActor*>& Actors);
    void ReleaseReflections();
    void ReleaseMirror(int32 Index);
    void EnsureReflectionMaterialTexture(const class UHarmonyDataAsset* Asset);
    void UpdateLumenFloor(const class UHarmonyDataAsset* Asset, const TArray<AActor*>& Actors, bool bWanted);
    void HandleLevelChanged(class ULevel* Level, class UWorld* World);
#if WITH_EDITOR
    void HandleObjectPropertyChanged(UObject* Object, struct FPropertyChangedEvent& Event);
    void HandleObjectTransacted(UObject* Object, const class FTransactionObjectEvent& Event);
    void HandleLevelActorListChanged(AActor* Actor);
#endif

    UPROPERTY(Transient)
    TObjectPtr<USceneCaptureComponent2D> SunCapture;
    UPROPERTY(Transient)
    TObjectPtr<UTextureRenderTarget2D> SunTarget;
    UPROPERTY(Transient)
    TObjectPtr<USceneCaptureComponent2D> UpCapture;
    UPROPERTY(Transient)
    TObjectPtr<UTextureRenderTarget2D> UpTarget;
    UPROPERTY(Transient)
    TObjectPtr<UTexture2D> HeightTexture;

    TWeakObjectPtr<const class UHarmonyDataAsset> HeightTextureAsset;
    // The actors with the Caster Tag, as of the last search: searched again only when something says they may have changed.
    TArray<TWeakObjectPtr<AActor>> TaggedCasters;
    bool bTaggedCastersDirty = true;
    FDelegateHandle LevelAddedHandle;
    FDelegateHandle LevelRemovedHandle;
#if WITH_EDITOR
    FDelegateHandle PropertyChangedHandle;
    FDelegateHandle TransactedHandle;
    FDelegateHandle ActorAddedHandle;
    FDelegateHandle ActorDeletedHandle;
#endif
    uint32 CaptureHash = 0u;        // what the two captures were taken of: recaptured when it changes
    bool bPublished = false;
    void* PublishedScene = nullptr;

    // Phase 16: the floor's material, and one mirrored capture of the casters per view that draws them (by view key)
    UPROPERTY(Transient)
    TObjectPtr<UTexture2D> ReflectionMaterialTexture;
    TWeakObjectPtr<const class UHarmonyDataAsset> ReflectionMaterialAsset;
    UPROPERTY(Transient)
    TArray<TObjectPtr<USceneCaptureComponent2D>> MirrorCaptures;
    UPROPERTY(Transient)
    TArray<TObjectPtr<UTextureRenderTarget2D>> MirrorTargets;
    TArray<UPTRINT> MirrorKeys;
    TArray<uint64> MirrorFrames;
    TArray<TWeakObjectPtr<AActor>> ReflectionCasters;
    FVector ReflectionPlanePoint = FVector::ZeroVector;
    FVector ReflectionPlaneNormal = FVector::UpVector;
    bool bReflectionActive = false;
    void* ReflectionScene = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<class UStaticMeshComponent> LumenFloor;
    UPROPERTY(Transient)
    TObjectPtr<class UMaterialInstanceDynamic> LumenFloorMaterial;
};
