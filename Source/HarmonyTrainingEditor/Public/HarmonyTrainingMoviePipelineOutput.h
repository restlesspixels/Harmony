// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "HarmonyCaptureRig.h"                  // EHarmonyFloorReflectionBake
#include "HarmonyTrainingEditorSubsystem.h"   // FHarmonyTrainingRequest
#include "MoviePipelineOutputBase.h"

#include "HarmonyTrainingMoviePipelineOutput.generated.h"

class UMoviePipeline;
class UMoviePipelineExecutorJob;
class UWorld;
struct FMoviePipelineMergerOutputFrame;

namespace HarmonyTrainingEditor
{
	struct FMrqCaptureState;
	struct FMrqView;
}

/** How UHarmonyTrainingMoviePipelineOutput builds the training init (Python: unreal.HarmonyTrainingInitMode.SAMPLER, ...). */
UENUM(BlueprintType)
enum class EHarmonyTrainingInitMode : uint8
{
	/** RotorSplatRenderTools' COLMAP points3D sampler, ported (HarmonyTraining::FColmapPointSampler): the same points3D as
	    Rotor's COLMAP output of the same render, then TrainSplat's Init "colmap" (KNN scale etc.). */
	Sampler,
	/** HarmonyTraining::FDepthSeedAccumulator (P2 depth seeding, notes/phase4_depth.md). */
	DepthSeed,
	/** No init (train with TrainingInit = a state dir / PLY, or "colmap" with ColmapInitSparseDir). */
	None
};

/** Whether a capture writes its dataset to the on-disk cache (Phase 6; Python: unreal.HarmonyTrainingCacheMode.PROJECT_DEFAULT ...). */
UENUM(BlueprintType)
enum class EHarmonyTrainingCacheMode : uint8
{
	/** UHarmonyTrainingSettings::bCacheCaptureDatasets (Project Settings > Plugins > Harmony Training). */
	ProjectDefault,
	Always,
	Never
};

/** What the last finished UHarmonyTrainingMoviePipelineOutput produced (UHarmonyTrainingMoviePipelineOutput::GetLastCaptureResult). */
USTRUCT(BlueprintType)
struct HARMONYTRAININGEDITOR_API FHarmonyTrainingCaptureResult
{
	GENERATED_BODY()

	/** The dataset was built (and, with depth seeding on, the init state dir written). */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") bool bSuccess = false;
	/** "OK: ..." or "ERROR: ..." (also logged). */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") FString Message;

	/** Frames with a camera pose and a FinalImage; dataset images; train / val split. */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 NumFrames = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 NumImages = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 NumTrain = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 NumVal = 0;
	/** Rendered (full) resolution and the dataset (downscaled) resolution of the first image. */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 FullWidth = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 FullHeight = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 Width = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 Height = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 Downscale = 1;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 EvalInterval = 8;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") double SceneScale = 0.0;
	/** FHarmonyTrainingDataset::SparseDir (points3D of Init "colmap"); may be empty. */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") FString SparseDir;

	/** "sampler", "depth_seed" or "none". */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") FString InitMode;
	/** The init (empty / 0 when off or failed): state dir, its points (sampler: <sparse>/points3D.txt; depth seed: the seeds),
	    point count. */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") FString InitStateDir;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") FString SeedPointsTxt;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 NumSeeds = 0;
	/** Sampler: its COLMAP sparse dir (cameras / images with POINTS2D / points3D, as Rotor writes them) and the peak memory of
	    the per-frame colour summaries it held (MiB). */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") FString SamplerSparseDir;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") double SamplerColorMiB = 0.0;
	/** Sampler: frames whose colour arrived before their pose (kept at full resolution until finalize; normally 0). */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 SamplerLateFrames = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 DepthViews = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 DepthCaptures = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 DepthUnsettled = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 ScaleClampRaised = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 ScaleClampLowered = 0;
	/** "views=.. tiles=.. ..." (FHarmonyTrainingDepthSeedStats::ToString) or the sampler's statistics. */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") FString SeedStats;

	/** Depth check against a reference points3D (VerifyPointsSparseDir): see the depth tool (HarmonyTrainingDepthCapture.h).
	    MatchRatio = match / (match + near + front); FrontRatio = front / (match + near + front). */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 VerifyViews = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int64 VerifyInside = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int64 VerifyMatch = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int64 VerifyNear = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int64 VerifyFront = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int64 VerifyBehind = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int64 VerifyNoDepth = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") double VerifyMatchRatio = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") double VerifyFrontRatio = 0.0;

	/** Debug COLMAP output (<dir>/sparse/0, <dir>/images, <dir>/images_<ds>); empty when off. With the dataset cache on
	    (and no DebugOutputDir) this is the cache folder. */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") FString DebugOutputDir;

	/** Phase 6: the dataset cache folder this capture wrote (harmony_dataset.json + COLMAP + PNGs); empty when off. */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") FString DatasetCacheDir;

	/** Phase 7: the dataset carries loss masks (excluded actors / capture radius); fraction of rendered pixels masked. */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") bool bMasked = false;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") double MaskedFraction = 0.0;

	/** Phase 7: TrainSplat options the capture implies ({"cull_radius": m, "cull_boxes": [[...]]}), merged under the
	    training request's own keys by the hand-off, StartTrainingFromLastCapture, TrainLastCaptureBlocking and the cache. */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") FString CaptureTrainingOptionsJson;

	/** Seconds: waiting for the image tasks after the last frame, depth replay + accumulation, seeding + init write. */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") double ImageWaitSeconds = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") double DepthSeconds = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") double SeedSeconds = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") double FinalizeSeconds = 0.0;

	/** bStartTraining: "" (not requested), "pending" (waiting for PIE to end), "started", or the start error. */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") FString TrainingStart;
};

/**
 * MRQ (legacy pipeline) output that turns a capture render into a HarmonyTraining dataset in memory, without the PNG /
 * COLMAP round trip (CONTRACT_P4.md section 4, CPP-MRQ; notes/phase4_mrq.md, phase4_critic.md 2.1, 2.2, 2.4).
 *
 *  1. Pose: at the centre temporal sample of every output frame (FWorldDelegates::OnWorldPostActorTick, because legacy
 *     MRQ never dispatches OnPostTick) the active cine camera of the player's view target is recorded: location (cm),
 *     UE right / up / forward, focal length, filmback. Same code and arithmetic as RotorSplatRenderTools'
 *     UColmapRenderMoviePipelineOutput::CaptureCameraState, ported (Harmony does not depend on Rotor).
 *  2. Pixels: the "FinalImage" pass is copied and converted on a worker task exactly like Rotor's CopyFinalImage
 *     (FImageCore::CopyImage to BGRA8 sRGB, alpha 255), then box-downscaled ((a+b+c+d+2)>>2 per channel, alpha 255) and
 *     stored as RGBA8 with R in the low byte. HasFinishedProcessing stays false until every task is done (critic 2.1).
 *  3. Dataset: frames are ordered and turned into COLMAP cameras exactly as Rotor's payload generator does (same
 *     frame conversion, quaternion, %.9g text); each FHarmonyTrainingCamera is built from that text's parsed values the
 *     way LoadColmapText does, then added with HarmonyTraining::AddDatasetImage. So the in-memory dataset is bit-identical
 *     to LoadColmapDataset(<Rotor sparse>, <images_N>) of the same render.
 *  4. Init (InitMode): every pose is replayed inside the MRQ (PIE) world with a transient SceneCapture2D (SCS_SceneDepth,
 *     R32F, Rotor's ReplayDepthCapture settings), time-sliced over HasFinishedProcessing calls.
 *     Sampler (default): all frames, in Rotor's order, feed HarmonyTraining::FColmapPointSampler (Rotor's points3D sampler,
 *     ported); each frame's colour was reduced to the sampler's candidate pixels when it was rendered
 *     (FColmapPointSampler::BuildColor), so no full-resolution image is kept. A worker task then selects the points, writes
 *     the COLMAP model (as Rotor) and builds TrainSplat's Init "colmap" from it into InitStateDir.
 *     DepthSeed: the seeded poses feed HarmonyTraining::FDepthSeedAccumulator; then FinalizeSeeds -> BuildInitFromSeeds ->
 *     WriteStateDir(InitStateDir) on a worker task.
 *  5. Optional debug output (DebugOutputDir): <dir>/sparse/0/{cameras,images,points3D}.txt (with the sampler: Rotor's
 *     model, POINTS2D included, plus <dir>/validation_report.txt), <dir>/images/<name>.png (the full-resolution PNG,
 *     byte-identical to Rotor's) and <dir>/images_<ds>/<name>.png.
 *  6. Hand-off: the dataset and the init are kept as the "last capture" (GetLastCaptureResult, StartTrainingFromLastCapture,
 *     TrainLastCaptureBlocking, ReleaseLastCapture: Python), and with bStartTraining they are handed to
 *     UHarmonyTrainingEditorSubsystem once PIE has ended.
 * One shot per job (frame names restart per shot). No mask support (Rotor's MaskType None).
 */
UCLASS(BlueprintType, meta = (DisplayName = "Harmony Training"))
class HARMONYTRAININGEDITOR_API UHarmonyTrainingMoviePipelineOutput : public UMoviePipelineOutputBase
{
	GENERATED_BODY()

public:
	UHarmonyTrainingMoviePipelineOutput();

	// ---- Dataset ----

	/** Training images are the render downscaled by this factor (1 or 2; 2 reproduces images_2 exactly). Intrinsics are
	    divided by it as LoadColmapText does. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dataset", meta = (ClampMin = 1, ClampMax = 2, UIMin = 1, UIMax = 2))
	int32 Downscale = 2;

	/** Train / val split of the name-sorted images: index k is val when k % EvalInterval == 0 (<= 0: no val). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dataset")
	int32 EvalInterval = 8;

	/** Record the pose at the temporal sample nearest the shutter centre (Rotor's default); false: the first sample. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dataset", AdvancedDisplay)
	bool bCaptureCenterTemporalSample = true;

	/** FHarmonyTrainingDataset::SparseDir: the points3D Init "colmap" reads (e.g. a Rotor sparse/0 for A/B runs). Empty: with
	    InitMode Sampler the sampler's sparse dir (DebugOutputDir/sparse/0, else <InitStateDir>/sparse/0); with DepthSeed the
	    debug sparse dir when DebugOutputDir is set (its points3D.txt holds the depth seeds); else none. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dataset", AdvancedDisplay)
	FString ColmapInitSparseDir;

	// ---- Init ----

	/** Sampler (default): replay depth per frame in the MRQ world, in Rotor's frame order, and run the ported Rotor COLMAP
	    sampler on it (same points3D as Rotor), then TrainSplat's Init "colmap" into InitStateDir. DepthSeed: P2's depth
	    seeding. None: no init. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Init")
	EHarmonyTrainingInitMode InitMode = EHarmonyTrainingInitMode::Sampler;

	/** Where the init state dir is written (TrainSplat / session Init). Empty: <Project>/Saved/HarmonyTraining/MRQ/<time>/init. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Init", meta = (EditCondition = "InitMode != EHarmonyTrainingInitMode::None"))
	FString InitStateDir;

	/** Game-thread time spent on depth captures (+ sampling) per finalize tick (the editor keeps ticking in between). At
	    least one pose per tick. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Init", AdvancedDisplay, meta = (ClampMin = 1.0, EditCondition = "InitMode != EHarmonyTrainingInitMode::None"))
	float DepthTimeSliceMs = 200.0f;

	// ---- Sampler (Rotor's COLMAP output settings, same defaults) ----

	/** Rotor UColmapRenderMoviePipelineOutput::TargetPoints (Rotor's default 500000). 16 Nov 2026: 1M by default - a 1M seed
	    scored 0.15 dB above 500k at the default cap_max 1M on the 1024 Showroom test (QUALITY_LOG "Seed count vs cap"). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sampler", meta = (ClampMin = 1, EditCondition = "InitMode == EHarmonyTrainingInitMode::Sampler"))
	int32 SamplerTargetPoints = 1000000;

	/** Rotor TileSizePx (detail tiles; coverage tiles are 4x). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sampler", AdvancedDisplay, meta = (ClampMin = 1, EditCondition = "InitMode == EHarmonyTrainingInitMode::Sampler"))
	int32 SamplerTileSizePx = 16;

	/** Rotor MaxDepthMeters: depth at or beyond this is background (and the capture's MaxViewDistanceOverride). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sampler", meta = (ClampMin = 1.0, EditCondition = "InitMode == EHarmonyTrainingInitMode::Sampler"))
	double SamplerMaxDepthMeters = 10000.0;

	/** Actor class-name substrings (incl. super classes) hidden from the sampler's depth replay. Empty (default) = Rotor:
	    nothing hidden. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sampler", AdvancedDisplay, meta = (EditCondition = "InitMode == EHarmonyTrainingInitMode::Sampler"))
	TArray<FString> SamplerHideActorClasses;

	// ---- Depth seed (InitMode DepthSeed) ----

	/** HarmonyTraining::ParseDepthSeedOptions keys (target_points, tile_px, ..., init_scale, init_opacity, rot_seed,
	    seed_views, max_views, settle_max, first_settle_max, hide_actor_classes, verify_views). Here settle_max and
	    first_settle_max default to 0 (one capture per pose, as Rotor) and eval_interval is ignored (EvalInterval wins). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Depth Seed", meta = (EditCondition = "InitMode == EHarmonyTrainingInitMode::DepthSeed"))
	FString DepthSeedOptionsJson;

	/** Optional COLMAP sparse dir whose points3D.txt checks the replayed depth (pose / FOV / world test; Rotor's points3D are
	    depth samples of the same render). verify_views views are checked. Empty: off. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Depth Seed", AdvancedDisplay, meta = (EditCondition = "InitMode == EHarmonyTrainingInitMode::DepthSeed"))
	FString VerifyPointsSparseDir;

	// ---- Training hand-off ----

	/** Start an in-editor training session (UHarmonyTrainingEditorSubsystem) on the dataset once the render's PIE has ended. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training")
	bool bStartTraining = false;

	/** Session Init: empty = the init state dir written above (sampler or depth seed); else "colmap" (points3D of the dataset's
	    SparseDir), a state dir or a PLY. The sampler's state dir uses init_scale / init_opacity / rot_seed of TrainingOptionsJson. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training", meta = (EditCondition = "bStartTraining"))
	FString TrainingInit;

	/** 0 = the project default (UHarmonyTrainingSettings::DefaultTrainingSteps). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training", meta = (ClampMin = 0, EditCondition = "bStartTraining"))
	int32 TrainingSteps = 0;

	/** TrainSplat's OptionsJson keys (HarmonyTrainingLibrary.h), merged over the project's DefaultTrainingOptionsJson. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training", meta = (EditCondition = "bStartTraining"))
	FString TrainingOptionsJson;

	/** Long package name of the asset saved when training is done (e.g. /Env_Showroom/Splats/SPL_Day). Empty: no asset. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training", meta = (EditCondition = "bStartTraining"))
	FString OutAsset;

	/** Optional PLY (+ _train.json / _loss.f32) written by the session. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training", AdvancedDisplay, meta = (EditCondition = "bStartTraining"))
	FString TrainingOutPly;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training", AdvancedDisplay, meta = (EditCondition = "bStartTraining"))
	bool bLivePreview = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training", AdvancedDisplay, meta = (EditCondition = "bStartTraining"))
	float FrameBudgetMs = 33.0f;

	// ---- Geometry (Phase 8) ----

	/** Store each view's depth at the dataset resolution (fp16 metres; 0 where masked, sky or not all sub-pixels have
	    depth) for depth supervision (TrainSplat option depth_lambda). Needs the depth replay (InitMode != None). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry")
	bool bCaptureDepth = true;

	/** With bCaptureDepth: replay every view once more with only the translucent surfaces (drawn opaque) and clear the
	    depth wherever glass lies in front of the opaque surface, so depth supervision never pulls glass towards what is
	    behind it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry", meta = (EditCondition = "bCaptureDepth"))
	bool bTranslucencyCoverage = true;

	/** Phase 8b: replay every view once more with Unreal's roughness buffer visualisation and clear the depth on glossy
	    pixels (roughness below GlossyRoughnessThreshold): reflections are learned as geometry behind the surface, which
	    depth supervision would otherwise forbid. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry", meta = (EditCondition = "bCaptureDepth"))
	bool bGlossyDepthMask = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry", meta = (ClampMin = 0.0, ClampMax = 1.0, EditCondition = "bCaptureDepth && bGlossyDepthMask"))
	float GlossyRoughnessThreshold = 0.3f;

	/** 15 Nov 2026: save every view's material buffers next to its depth, at the dataset resolution (cache channels,
	    8-bit PNGs named like the images; values are linear, not tone-mapped or gamma-encoded, except as stated):
	    roughness_<ds>   grey, Unreal's roughness: roughness = v / 255;
	    reflectance_<ds> RGB, the specular reflectance at normal incidence F0 = lerp(0.08 * Specular, BaseColor, Metallic)
	                     (what the surface mirrors: ~0.04 for dielectrics, the base colour for metals), stored as its
	                     square root: F0 = (v / 255)^2 (0.04 -> 51);
	    normal_<ds>      RGB, the unit world normal in Unreal's axes (X, Y, Z = R, G, B): n = 2 * v / 255 - 1
	                     (0.5 + 0.5 n; pixels without a surface are 128, 128, 128). In the dataset's COLMAP frame the
	                     normal is (n.X, -n.Z, -n.Y).
	    depth_opaque_<ds> (with bTranslucencyCoverage) fp16 metres like depth_<ds>, but before the depth is cleared
	                     behind glass: pixels with depth here and none in depth_<ds> are seen through glass.
	    Glass is translucent, so its pixels carry the opaque surface behind it. Five extra scene captures per view at
	    the dataset resolution, read back as linear floats (roughness / metallic / specular: buffer visualisation
	    materials through SCS_FinalColorHDR; base colour and normal: the GBuffer). For reflection-aware training
	    (e.g. seeding virtual reflection points, reflectivity-dependent opacity) without a separate render. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry", meta = (EditCondition = "bCaptureDepth"))
	bool bCaptureMaterialBuffers = true;

	// ---- Background (Phase 7b) ----

	/** Render each view's sky (no primitives: atmosphere / clouds / fog only) and train with it composited behind the
	    splats (render = splats + transmittance * sky): sky pixels are no longer painted with giant far splats, glass in
	    front of the sky learns only its own tint and reflection, and UE's sky shows through at runtime. Far geometry stays
	    splats (masked beyond a finite capture radius, Phase 7). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background")
	bool bCaptureBackground = true;

	/** Lock the exposure of every view at LockedExposureEV100 (an unbound post process volume of the top priority, spawned
	    in the render world for the capture only: Min = Max brightness in the units the project's luminance range uses, no
	    exposure compensation, a flat exposure compensation curve). Off: the scene's own exposure (it must already be fixed). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exposure")
	bool bLockExposure = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exposure", meta = (EditCondition = "bLockExposure"))
	float LockedExposureEV100 = 10.5f;

	/** With Lock Exposure off: the EV100 the scene itself locks the views at, when known (the capture job sets it from the
	    scene's post process volume). Recorded with the lighting probe only; it changes nothing in the render. Below -999:
	    unknown (the probe then estimates its exposure from its own mean luminance). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exposure", AdvancedDisplay, meta = (EditCondition = "!bLockExposure"))
	float SceneExposureEV100 = -1000.0f;

	/** Vignette, bloom, flares, fringe, grain and motion blur off during the capture (they change from view to view). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exposure")
	bool bDisableLensEffects = false;

	// ---- Lighting probe (Phase 13) ----

	/** Also capture the scene's lighting from the rig's centre (a 360 degree HDR probe, lighting_probe.hdr in the dataset
	    cache): the trained asset carries it and AHarmonyActor lights the level with it. Needs the dataset cache. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting Probe")
	bool bCaptureLightingProbe = true;

	/** Width of the probe's long-lat image (its height is half). 2048 keeps reflections on glossy paint sharp; 1024 is
	    plenty for lighting alone at a quarter of the size, 4096 is four times it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting Probe", meta = (ClampMin = 128, ClampMax = 4096, EditCondition = "bCaptureLightingProbe"))
	int32 LightingProbeResolution = 2048;

	/** Height (cm) of the probe above the capture rig's origin: where the lit object will stand. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting Probe", meta = (EditCondition = "bCaptureLightingProbe"))
	float LightingProbeHeight = 120.0f;

	/** Also capture the ground around the rig's origin from above (the shadow proxy) and measure the sun's share of its
	    light, for the Harmony actor's shadow catcher. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting Probe", meta = (EditCondition = "bCaptureLightingProbe"))
	bool bCaptureShadowProxy = true;

	/** Phase 16: also bake the floor's material around the rig's origin (roughness, specular, metallic from above, over the
	    shadow proxy's square) for the Harmony actor's floor reflections. Auto: only when a reflection would show; the
	    decision and its numbers go to the log and lighting_probe.json. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting Probe", meta = (EditCondition = "bCaptureLightingProbe"))
	EHarmonyFloorReflectionBake FloorReflection = EHarmonyFloorReflectionBake::Auto;

	/** Capture the probe twice and store how far the two are apart (the noise floor, in lighting_probe.json). Bake
	    Variants judges every option's probe against the base capture's floor; the one-click job turns this on for the
	    base capture only. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting Probe", meta = (EditCondition = "bCaptureLightingProbe"))
	bool bMeasureLightingProbeNoise = true;

	// ---- Dataset cache (Phase 6) ----

	/** Write the dataset (COLMAP text with the sampler's points, downscaled PNGs, harmony_dataset.json) to
	    <project DatasetCacheRoot>/<name>, so it can be trained again without rendering (TrainFromCache /
	    StartTrainingFromCache). Ignored when DebugOutputDir is set (that folder is the dataset then). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dataset Cache")
	EHarmonyTrainingCacheMode CacheDataset = EHarmonyTrainingCacheMode::ProjectDefault;

	/** Cache folder name prefix (a timestamp is appended). Empty: the MRQ job name, else the map name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dataset Cache")
	FString DatasetCacheName;

	// ---- Debug ----

	/** Debug switch (default off): also write the dataset as a COLMAP text model + PNGs into this absolute folder. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FString DebugOutputDir;

	/** With DebugOutputDir: also write the full-resolution PNGs (<dir>/images; byte-identical to Rotor's). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDebugWriteFullResImages = true;

	// ---- Python / Blueprint: the last capture of this process ----

	/** Result of the last finished capture (bSuccess false when there was none). */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	static FHarmonyTrainingCaptureResult GetLastCaptureResult();

	/** Captures finished in this process, successful or not (each one replaces GetLastCaptureResult). A render that ends
	    without changing it never reached the end of this output (e.g. Play In Editor did not start): the last result is
	    then an earlier render's. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	static int64 GetLastCaptureCount();

	/** True while the last capture's in-memory dataset is held (until ReleaseLastCapture or the next capture). */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	static bool HasLastCapture();

	/** UHarmonyTrainingEditorSubsystem::StartTrainingWithDataset on the last capture. Request.Init "" = its init state
	    dir; Request.ColmapSparseDir / ImagesDir are ignored. Returns "" (started) or the error. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	static FString StartTrainingFromLastCapture(const FHarmonyTrainingRequest& Request);

	/** HarmonyTraining::TrainSplatFromDataset (blocking, TrainSplat's exact path) on the last capture. Init "" = its
	    init state dir. Returns TrainSplat's "OK: ..." / "ERROR: ..." line. Refused while a subsystem session runs. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	static FString TrainLastCaptureBlocking(const FString& Init, int32 Steps, const FString& OutPly, const FString& OptionsJson);

	/** Phase 6: loads the last capture's dataset cache from disk (LoadColmapDataset, val images included) and compares it
	    with the in-memory dataset exactly (cameras, split, scene scale, every image byte). Returns "" when identical. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	static FString VerifyLastCaptureCache();

	/** Drops this process's reference to the last capture's dataset (a running session keeps its own). */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	static void ReleaseLastCapture();

#if WITH_EDITOR
	// UMoviePipelineSetting (public there, MoviePipelineSetting.h:99-104)
	virtual FText GetDisplayText() const override;
	virtual FText GetFooterText(UMoviePipelineExecutorJob* InJob) const override;
#endif

protected:
	// UMoviePipelineSetting
	virtual void SetupForPipelineImpl(UMoviePipeline* InPipeline) override;
	virtual void TeardownForPipelineImpl(UMoviePipeline* InPipeline) override;
	// UMoviePipelineOutputBase
	virtual void OnPostTickImpl() override;
	virtual void OnReceiveImageDataImpl(FMoviePipelineMergerOutputFrame* InMergedOutputFrame) override;
	virtual void BeginFinalizeImpl() override;
	virtual bool HasFinishedProcessingImpl() override;
	virtual void FinalizeImpl() override;
	virtual void OnPipelineFinishedImpl() override;
	// UObject
	virtual void BeginDestroy() override;

private:
	void HandleWorldPostActorTick(UWorld* World, ELevelTick TickType, float DeltaSeconds);
	void CapturePoseIfWanted();
	bool BuildDataset();
	void CaptureLightingProbeForDataset();
	/** Phase 7: reads the render world's capture rig (excluded actors, capture radius) and turns masks on when needed. */
	void SetupMasks(UWorld* World);
	/** Phase 7: one view's mask (excluded-actor replay + radius test) -> dataset alpha, mask PNG, masked depth = 0. */
	bool ApplyViewMask(UWorld* World, const HarmonyTrainingEditor::FMrqView& View, TArray<float>& InOutDepthCm, FString& OutError);
	/** Phase 8: one view's depth at the dataset resolution -> Dataset->Depths. */
	void StoreViewDepth(const HarmonyTrainingEditor::FMrqView& View, const TArray<float>& DepthCm);
	/** Phase 8: translucency sweep (time-sliced like TickDepth). Begin returns false when there is nothing to do. */
	bool BeginTranslucency(UWorld* World);
	void TickTranslucency();
	void EndTranslucency();
	/** Phase 7b: one view's far scene / sky -> Dataset->Backgrounds (+ cache PNG). Phase 8b: glossy pixels -> depth 0. */
	bool CaptureViewBackground(UWorld* World, const HarmonyTrainingEditor::FMrqView& View, const TArray<float>& FarMaskFull, FString& OutError);
	bool ApplyViewGlossy(UWorld* World, const HarmonyTrainingEditor::FMrqView& View, FString& OutError);
	/** 15 Nov 2026: one view's roughness / reflectance / normal buffers -> the cache (bCaptureMaterialBuffers). */
	bool CaptureViewMaterials(UWorld* World, const HarmonyTrainingEditor::FMrqView& View, FString& OutError);
	FString ResolveInitDir() const;
	bool BeginDepth();
	void TickDepth();
	void LaunchSeedTask();
	void FinishCapture();
	void ResetState();

	/** All per-render state (frames, tasks, dataset, depth replay, accumulator); null outside a render. */
	TSharedPtr<HarmonyTrainingEditor::FMrqCaptureState> State;
};
