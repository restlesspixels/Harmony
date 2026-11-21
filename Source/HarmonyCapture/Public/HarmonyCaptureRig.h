// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HarmonyVariantSource.h"
#include "HarmonyCaptureRig.generated.h"

class UCineCameraComponent;
class UHarmonyCaptureSampleMarker;
class UHarmonyCaptureBoundsComponent;
class UHarmonyCaptureGridSampler;
class UHarmonyCaptureRadialSampler;

/** Phase 7: how far from the world origin a capture trains (Python: unreal.HarmonyCaptureRadiusMode.PROJECT_DEFAULT ...). */
UENUM(BlueprintType)
enum class EHarmonyCaptureRadiusMode : uint8
{
    /** Project Settings > Plugins > Harmony Training (bTrainToInfinity / DefaultCaptureRadiusMeters). */
    ProjectDefault,
    /** Train everything the cameras see, however far (sky and distant scenery become splats). */
    Infinite,
    /** CaptureRadiusMeters from the world origin: pixels showing anything farther (and the sky) are masked out of the
        loss, no init point is placed there, and splats that drift beyond it are removed. UE draws its own sky / scenery
        behind the splats at runtime. */
    Custom
};

/** Phase 9: which shots the rig's ShotIndex walks (Python: unreal.HarmonyCaptureShotSet.ALL ...). */
UENUM(BlueprintType)
enum class EHarmonyCaptureShotSet : uint8
{
    /** The grid / radial shots, then every custom view cluster. */
    All,
    /** Only the grid / radial shots (the original rig). */
    SamplesOnly,
    /** Only the custom view clusters (refinement captures). */
    CustomViewsOnly
};

/** How a capture locks its exposure (every view must share one exposure, or auto-exposure changes are learned as colour). */
UENUM(BlueprintType)
enum class EHarmonyCaptureExposure : uint8
{
    /** Use the scene's own exposure if an unbound post process volume already locks it (Min = Max, or Manual with
        Apply Physical Camera Exposure off) and no volume has an exposure compensation curve; when the scene is
        otherwise fixed (a curve, or Manual with the physical camera on) lock at the EV100 the viewport shows; with no
        lock at all, lock at ExposureEV100. */
    Auto,
    /** Always lock at ExposureEV100. */
    Locked,
    /** Leave the scene's exposure alone (only if you know it is fixed). */
    Scene
};

/** The rig's capture-and-train settings (defaults suit most scenes). */
/** Phase 13: whether a variant option keeps a lighting probe of its own. */
UENUM(BlueprintType)
enum class EHarmonyVariantProbeStore : uint8
{
    /** Keep it when the option's lighting differs from the base's by more than the capture's noise (a time of day does;
        a paint colour mostly does not). Otherwise the option stores none and uses the base's probe. */
    Auto,
    /** Always keep the option's own probe. */
    AlwaysStore,
    /** Never: the option uses the base's probe. */
    NeverStore
};

/** Phase 16: whether a capture bakes the floor's material so that a Harmony actor can reflect a vehicle in it. */
UENUM(BlueprintType)
enum class EHarmonyFloorReflectionBake : uint8
{
    /** Bake it when a reflection would show: when enough of the floor within a few metres of the stand is smooth enough
        (low roughness), weighted by its reflectance. A polished floor or a wet road: yes; gravel, dirt, grass: no. The
        capture log and lighting_probe.json give the numbers. */
    Auto,
    /** Always bake it. */
    Always,
    /** Never: no reflections, no extra data. */
    Never
};

/** Phase 13: the lighting probe rule of one variant option (an option that is not listed: Auto). */
USTRUCT(BlueprintType)
struct HARMONYCAPTURE_API FHarmonyVariantProbeOverride
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture")
    FName Set;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture")
    FName Option;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture")
    EHarmonyVariantProbeStore Store = EHarmonyVariantProbeStore::Auto;
};

USTRUCT(BlueprintType)
struct HARMONYCAPTURE_API FHarmonyCaptureJobSettings
{
    GENERATED_BODY()

    /** Splat asset the training writes. Empty: <map's content root>/Splats/SPL_<map name>. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture")
    FString OutputAsset;

    /** Square render size of each shot (the training images are half of it). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture", meta=(ClampMin="256", ClampMax="4096"))
    int32 Resolution = 1024;

    /** Temporal samples per shot (the pose does not move: more samples settle Lumen / denoise). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture", AdvancedDisplay, meta=(ClampMin="1", ClampMax="64"))
    int32 TemporalSamples = 8;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture")
    EHarmonyCaptureExposure Exposure = EHarmonyCaptureExposure::Auto;

    /** Locked exposure (EV100): interiors ~6-9, overcast ~11-12, sun ~13-15. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture", meta=(EditCondition="Exposure != EHarmonyCaptureExposure::Scene"))
    float ExposureEV100 = 10.5f;

    /** Bloom, vignette, flares, fringe, grain and motion blur off during the capture (they change from view to view). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture", AdvancedDisplay)
    bool bDisableLensEffects = true;

    /** Also capture the scene's lighting from the rig's origin (a 360 degree HDR probe): the trained asset carries it,
        one per variant option too, and a Harmony actor lights the level with it (a vehicle put into the splat gets the
        scene's own lighting). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture")
    bool bCaptureLightingProbe = true;

    /** Width of the lighting probe's long-lat image (the height is half). 2048 keeps reflections on glossy paint sharp
        (the Harmony actor's sky light filters it at 512 a face); 1024 is plenty for lighting alone at a quarter of the
        size, 4096 is four times it. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture", AdvancedDisplay, meta=(ClampMin="128", ClampMax="4096", EditCondition="bCaptureLightingProbe"))
    int32 LightingProbeResolution = 2048;

    /** Height (cm) of the lighting probe above the rig's origin: where the lit object will stand. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture", AdvancedDisplay, meta=(EditCondition="bCaptureLightingProbe"))
    float LightingProbeHeight = 120.0f;

    /** With the lighting probe, also capture the ground around the rig's origin from above (a heightfield, 24 m square)
        and measure the sun's share of its light: a Harmony actor uses them to let the splats catch the shadows of what
        is put into the environment. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture", AdvancedDisplay, meta=(EditCondition="bCaptureLightingProbe"))
    bool bCaptureShadowProxy = true;

    /** With the lighting probe, also bake the floor's material around the rig's origin (roughness, specular, metallic,
        from above, over the shadow proxy's square) so that a Harmony actor reflects a vehicle in a shiny floor. Auto:
        only when a reflection would show (a polished floor, a wet road; not gravel or dirt). Variants share it. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture", AdvancedDisplay, meta=(EditCondition="bCaptureLightingProbe"))
    EHarmonyFloorReflectionBake FloorReflection = EHarmonyFloorReflectionBake::Auto;

    /** Bake Variants: per variant option, whether it keeps a lighting probe of its own. An option that is not listed is
        Auto: its probe is kept only when it differs from the base's by more than the capture's noise (the log has the
        numbers of every option). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture", AdvancedDisplay, meta=(EditCondition="bCaptureLightingProbe"))
    TArray<FHarmonyVariantProbeOverride> VariantLightingProbes;

    /** Training steps (0: Project Settings > Harmony Training default). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture", meta=(ClampMin="0"))
    int32 TrainingSteps = 0;

    /** Extra trainer options (JSON) over the project defaults. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture", AdvancedDisplay)
    FString TrainingOptionsJson;

    /** Bake Variants: keep the Output Asset and this rig's last base capture, and only capture and bake the options
        (to add or redo options). Only while the base look of the scene is unchanged since that capture. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture", AdvancedDisplay)
    bool bReuseBaseForVariants = false;

    /** Bake Variants: training steps per option, from the base splat (a fifth SH-only warm-up, then the splats whose
        opacity / position / shape the option needs are unlocked for the rest). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture", AdvancedDisplay, meta=(ClampMin="1000"))
    int32 VariantTrainingSteps = 25000;

    /** Bake Variants: a point joins an option's delta when its colour moves more than this (0..1). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture", AdvancedDisplay, meta=(ClampMin="0.0", ClampMax="0.5"))
    float VariantThreshold = 0.01f;
};

/** Phase 9: a hand-placed viewpoint that needs more training data. */
USTRUCT(BlueprintType)
struct HARMONYCAPTURE_API FHarmonyCaptureCustomView
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom View")
    FString Label;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom View")
    bool bEnabled = true;

    /** Camera position and orientation (world). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom View")
    FVector Location = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom View")
    FRotator Rotation = FRotator::ZeroRotator;

    /** Extra cameras on a ring around the view (perpendicular to its forward axis), all aimed at the same focus point:
        one viewpoint alone gives the trainer no parallax. 0 = the view only. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom View", meta=(ClampMin="0", ClampMax="64"))
    int32 ClusterSize = 8;

    /** Ring radius (cm). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom View", meta=(ClampMin="0.0"))
    float ClusterRadius = 30.0f;

    /** Distance (cm) along the view's forward axis of the point the ring cameras look at. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom View", meta=(ClampMin="10.0"))
    float FocusDistance = 400.0f;
};

UENUM(BlueprintType)
enum class EHarmonyCaptureSamplingPattern : uint8
{
    /** One position every GridSpacing cm (the original rig; shots: NumberOfYawSteps x NumberOfPitchSteps). */
    Grid,
    Radial,
    /** A set number of rows along each axis of every include box, spread edge to edge, each position jittered inside
        its cell; per position RowsYawSteps yaws (random start) at each of RowsTilts, outer rows looking inward. */
    Rows UMETA(DisplayName="Rows (jittered)")
};

/**
 * Gaussian-splat capture rig: a square-filmback cine camera that visits every camera sample in turn as the Sequencer
 * keys ShotIndex 0 .. TotalShots-1 (one pose per output frame).
 *
 * A port of RotorSplatRenderTools' ASpatialCaptureRig with identical behaviour: the same settings give the same marker
 * positions and the same camera pose for every ShotIndex, bit for bit (Scripts/EnvGen/compare_rig_poses.py checks it).
 * Property names match the Rotor rig so the build scripts read the same; only the class names differ.
 *
 * Sample layout per location (marker): shot 0 looks straight up (pitch +90), the last shot straight down (pitch -90),
 * the shots between run over NumberOfYawSteps yaws for each of the NumberOfPitchSteps-2 inner pitches.
 */
UCLASS(HideCategories=("Rendering", "Physics", "Tags", "AssetUserData", "Activation", "Navigation", "Replication", "Collision", "HLOD", "Networking", "Actor", "Input", "LOD", "LevelInstance"))
class HARMONYCAPTURE_API AHarmonyCaptureRig : public AActor
{
    GENERATED_BODY()

public:
    AHarmonyCaptureRig();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<USceneComponent> RootSceneComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UCineCameraComponent> MainCamera;

    /** Continuous Sequencer input. It is never overwritten by derived/clamped state. */
    UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, BlueprintSetter=SetShotIndex, Category="Camera Shot", meta=(ClampMin="0", UIMin="0"))
    float ShotIndex = 0.0f;

    /** Rounded and clamped camera pose used by the rig. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera Shot")
    int32 ResolvedShotIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Rotation", meta=(ClampMin="1", UIMin="1"))
    int32 NumberOfYawSteps = 8;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Rotation", meta=(ClampMin="2", UIMin="2"))
    int32 NumberOfPitchSteps = 5;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera Shot")
    int32 LocationIndex = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera Shot")
    int32 YawIndex = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera Shot")
    int32 PitchIndex = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera Shot")
    int32 TotalShots = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Locations")
    EHarmonyCaptureSamplingPattern SamplingPattern = EHarmonyCaptureSamplingPattern::Grid;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Locations")
    bool bShowCameraSampleMarkers = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Locations", meta=(EditConditionHides, EditCondition="SamplingPattern == EHarmonyCaptureSamplingPattern::Grid", ClampMin="1.0", UIMin="1.0"))
    float GridSpacing = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Locations", meta=(EditConditionHides, EditCondition="SamplingPattern == EHarmonyCaptureSamplingPattern::Radial", ClampMin="1", UIMin="1"))
    int32 RadialHeightSteps = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Locations", meta=(EditConditionHides, EditCondition="SamplingPattern == EHarmonyCaptureSamplingPattern::Radial", ClampMin="1", UIMin="1", ClampMax="360", UIMax="360"))
    int32 RadialAngleSteps = 10;

    // ---- Rows sampling (13 Nov 2026): rows per axis, jittered cells, few shots per position ----

    /** Rows of camera positions along each axis of every include box (X, Y, Z in the box's frame), spread edge to edge:
        3 gives one row on each face and one in the middle. Resizing the box changes the spacing, not the camera count. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Locations", meta=(EditConditionHides, EditCondition="SamplingPattern == EHarmonyCaptureSamplingPattern::Rows", ClampMin="1", UIMin="1"))
    FIntVector RowsPerAxis = FIntVector(9, 9, 5);

    /** How far each position may move from its row point, as a fraction of the row spacing: 0 = a regular grid, 1 =
        anywhere in its cell. Positions never leave the box (the edge rows only move inward). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Locations", meta=(EditConditionHides, EditCondition="SamplingPattern == EHarmonyCaptureSamplingPattern::Rows", ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
    float RowJitter = 1.0f;

    /** Seed of the jitter and the start yaws. The same seed and settings give the same poses (variant captures need
        the base's exact poses). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Locations", meta=(EditConditionHides, EditCondition="SamplingPattern == EHarmonyCaptureSamplingPattern::Rows"))
    int32 RandomSeed = 1;

    /** A position that would sit inside scene geometry is drawn again inside its cell (and dropped if no free spot is
        found). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Locations", meta=(EditConditionHides, EditCondition="SamplingPattern == EHarmonyCaptureSamplingPattern::Rows"))
    bool bAvoidGeometry = true;

    /** Rows: positions, shots and an estimate of the capture time (read only). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera Locations", meta=(EditConditionHides, EditCondition="SamplingPattern == EHarmonyCaptureSamplingPattern::Rows"))
    FString RowsSummary;

    /** Rows: yaws per tilt at every position, evenly spaced. 5 at FOV 90 overlaps neighbours by 18 degrees. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Rotation", meta=(EditConditionHides, EditCondition="SamplingPattern == EHarmonyCaptureSamplingPattern::Rows", ClampMin="1", UIMin="1"))
    int32 RowsYawSteps = 5;

    /** Rows: camera tilts in degrees (+ looks up). -60 / 0 / +60 with FOV 90 covers the whole sphere. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Rotation", meta=(EditConditionHides, EditCondition="SamplingPattern == EHarmonyCaptureSamplingPattern::Rows"))
    TArray<float> RowsTilts = { -60.0f, 0.0f, 60.0f };

    /** Rows: every position starts its yaws at a random angle, so frame edges do not line up across positions. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Rotation", meta=(EditConditionHides, EditCondition="SamplingPattern == EHarmonyCaptureSamplingPattern::Rows"))
    bool bRandomStartYaw = true;

    /** Rows: the bottom row (near the floor) skips downward tilts, so it looks out along the floor. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Rotation", meta=(EditConditionHides, EditCondition="SamplingPattern == EHarmonyCaptureSamplingPattern::Rows"))
    bool bSkipDownTiltsOnBottomRow = true;

    /** Rows: the top row skips upward tilts. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Rotation", meta=(EditConditionHides, EditCondition="SamplingPattern == EHarmonyCaptureSamplingPattern::Rows"))
    bool bSkipUpTiltsOnTopRow = true;

    /** Rows: positions on the box's outer rows (X or Y) keep only shots heading within this many degrees of the box
        centre (where the product stands): they look in and across, not out of the scene. 180 = keep every shot. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Rotation", meta=(EditConditionHides, EditCondition="SamplingPattern == EHarmonyCaptureSamplingPattern::Rows", ClampMin="0.0", ClampMax="180.0", UIMin="0.0", UIMax="180.0"))
    float OuterRowsInwardCone = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Rotation", meta=(ClampMin="1.0", ClampMax="170.0"))
    float FOV = 90.0f;

    /** Generated marker components. Serialized so the rig remains usable after reopening a map. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Camera Locations")
    TArray<UHarmonyCaptureSampleMarker*> CameraSampleMarkers;

    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Camera Locations")
    TArray<UHarmonyCaptureBoundsComponent*> IncludeBounds;

    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Camera Locations")
    TArray<UHarmonyCaptureBoundsComponent*> ExcludeBounds;

    // ---- Custom views (Phase 9, refinement) ----

    /** Hand-placed viewpoints (saved with the map, so they are shared through source control). Each adds 1 + ClusterSize
        shots after the grid / radial shots. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Views")
    TArray<FHarmonyCaptureCustomView> CustomViews;

    /** All shots, the samples only, or the custom views only (a refinement capture). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Views")
    EHarmonyCaptureShotSet ShotSet = EHarmonyCaptureShotSet::All;

    /** Appends the level viewport's current camera as a custom view (editor only). */
    UFUNCTION(CallInEditor, Category="Custom Views")
    void CaptureCurrentView();

    /** Appends a custom view (Python / Blueprint). Regenerates the shot list. */
    UFUNCTION(BlueprintCallable, Category="Custom Views")
    void AddCustomView(const FVector& Location, const FRotator& Rotation, const FString& Label, int32 ClusterSize = 8);

    /** Shots of the custom view clusters (enabled views only). */
    UFUNCTION(BlueprintPure, Category="Custom Views")
    int32 GetNumCustomShots() const;

    /** World pose of custom shot Index (0-based over the enabled views' clusters). False when out of range. */
    UFUNCTION(BlueprintCallable, Category="Custom Views")
    bool GetCustomShotPose(int32 Index, FVector& OutLocation, FRotator& OutRotation) const;

    // ---- Training (Phase 7, read by the Harmony Training MRQ output; notes/phase7.md) ----

    /** Actors that are never trained (e.g. the configurator car's orbit zone): every pixel showing them is masked out of
        the loss (no gradient, so other views are not pushed to "see through" them), no init point lands on them, and
        splats inside their bounds are removed during training. Actors tagged ExcludeActorTag count too. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Training")
    TArray<TObjectPtr<AActor>> ExcludedActors;

    /** Actor tag that marks an actor as excluded (see ExcludedActors). Empty: tags are not used. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Training")
    FName ExcludeActorTag = TEXT("HarmonyExclude");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Training")
    EHarmonyCaptureRadiusMode CaptureRadiusMode = EHarmonyCaptureRadiusMode::ProjectDefault;

    /** With CaptureRadiusMode Custom: metres from the world origin. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Training", meta=(ClampMin="1.0", EditCondition="CaptureRadiusMode == EHarmonyCaptureRadiusMode::Custom"))
    float CaptureRadiusMeters = 2000.0f;

    /** Every excluded actor of this rig's world: ExcludedActors plus the actors carrying ExcludeActorTag. */
    UFUNCTION(BlueprintCallable, Category="Training")
    TArray<AActor*> GetExcludedActors() const;

    /** P10 configurable variants (experimental): where the scene's configuration options come from (Material Swaps
        built in; Level Variant Sets in the HarmonyVariantManager module; or your own UHarmonyVariantSource). A variant
        bake captures the base and every option of VariantSetsToBake with the same poses and stores each option as a sparse
        appearance delta in the trained Harmony asset (UHarmonyComponent::SetVariant switches them). */
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Variants")
    TObjectPtr<UHarmonyVariantSource> VariantSource;

    /** Variant sets to bake (empty: all of the source's sets). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variants")
    TArray<FName> VariantSetsToBake;

    /** How the buttons below render and train (sensible defaults: most captures need no changes here). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture", meta=(ShowOnlyInnerProperties))
    FHarmonyCaptureJobSettings Capture;

    /** Renders every shot of this rig (Movie Render Queue, in the editor, no setup needed) and trains the splat into
        Capture.OutputAsset with a live preview. */
    UFUNCTION(CallInEditor, Category="Capture", meta=(DisplayName="Capture and Train"))
    void CaptureAndTrain();

    /** Renders every shot into a dataset cache without training (train it later from the cache). */
    UFUNCTION(CallInEditor, Category="Capture", meta=(DisplayName="Capture Only"))
    void CaptureOnly();

    /** Experimental (P10): captures the base and every option of the variant source with the same shots, trains the
        base and bakes each option into the asset as a sparse appearance delta. */
    UFUNCTION(CallInEditor, Category="Capture", meta=(DisplayName="Bake Variants"))
    void BakeVariants();

    /** Trains Capture.OutputAsset again from this rig's last capture (the dataset cache), without rendering: for new
        training steps or options. */
    UFUNCTION(CallInEditor, Category="Capture", meta=(DisplayName="Retrain From Last Capture"))
    void RetrainFromLastCapture();

    /** Builds the low-end fallback meshes (simplified, textured static meshes; Docs/Dev/SplatToMesh_M3_NativePort.md)
        from this rig's last capture and writes them next to Capture.OutputAsset. Settings: Project Settings, Plugins,
        Harmony Fallback Mesh. Takes minutes and blocks the editor while it runs. */
    UFUNCTION(CallInEditor, Category="Capture", meta=(DisplayName="Build Fallback Mesh From Last Capture"))
    void BuildFallbackFromLastCapture();

    /** Stops a running capture / bake after the current step. */
    UFUNCTION(CallInEditor, Category="Capture", meta=(DisplayName="Cancel Capture"))
    void CancelCapture();

    /** Rebuilds the camera sample markers from the include / exclude bounds and re-applies the current ShotIndex. */
    UFUNCTION(BlueprintCallable, CallInEditor, Category="Camera Locations")
    void GenerateCameraSamples();

    UFUNCTION(BlueprintSetter, CallInEditor, Category="Camera Shot")
    void SetShotIndex(float InShotIndex);

    UFUNCTION(BlueprintPure, Category="Camera Shot")
    int32 GetShotsPerLocation() const;

protected:
    /** A rig placed in the editor starts with the Rows pattern and a 16 x 16 x 2.5 m include box from 20 cm above
        its origin (a rig loaded from a map, or one with include boxes already, is left alone). */
    virtual void PostActorCreated() override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    void UpdateShotIndexMetaData();
#endif

private:
    int32 GenerateRowsSamples();
    int32 GetMarkerShots(const UHarmonyCaptureSampleMarker* Marker) const;
    void ApplySamplingSettingsToMarkers();
    void RestoreCameraSampleMarkerReferences();
    void AttachBoundsToRoot();
    void SetCameraTransform();
    void UpdateCameraSampleMarkerVisibility();

    UPROPERTY()
    TObjectPtr<UHarmonyCaptureGridSampler> GridSampler;

    UPROPERTY()
    TObjectPtr<UHarmonyCaptureRadialSampler> RadialSampler;
};
