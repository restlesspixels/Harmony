// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HarmonyTrainingTypes.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/WeakObjectPtr.h"

class ASceneCapture2D;
class UTextureRenderTarget2D;
class UWorld;

/**
 * Phase 4 P2 pose-replay depth collector (CONTRACT_P4.md section 3, CPP-DEPTH).
 * Replays COLMAP poses with a transient ASceneCapture2D (SCS_SceneDepth into an RTF_R32f target, full resolution, no
 * post-processing / TAA / motion blur: planar view Z in cm, one unjittered sample), the same scheme as Rotor's
 * UColmapRenderMoviePipelineOutput::EnsureDepthCapture / ReplayDepthCapture (ColmapRenderMoviePipelineOutput.cpp:172-336),
 * re-implemented here so Harmony does not depend on RotorSplatRenderTools. Game thread only, blocking
 * (FlushRenderingCommands per read). The Phase 4 MRQ output can reuse FHarmonyDepthCapture with its PIE world.
 */
namespace HarmonyTrainingEditor
{
	struct FDepthCaptureSettings
	{
		/** USceneCaptureComponent::MaxViewDistanceOverride (cm); farther content is culled and reads as sky. */
		float MaxDepthCm = 1.0e6f;
		/** Actors whose class (or a super class) name contains one of these are hidden (the capture rig, its markers). */
		TArray<FString> HideActorClasses;
		/** After each capture, end/begin a render-thread frame (as the headless trainer's PumpFrame): commandlets have no
		    engine loop, and Nanite streaming / pooled RDG resources advance per frame. Leave false inside a ticking editor. */
		bool bPumpFrames = false;
	};

	class HARMONYTRAININGEDITOR_API FHarmonyDepthCapture
	{
	public:
		FHarmonyDepthCapture();
		~FHarmonyDepthCapture();
		FHarmonyDepthCapture(const FHarmonyDepthCapture&) = delete;
		FHarmonyDepthCapture& operator=(const FHarmonyDepthCapture&) = delete;

		/** Spawns the transient capture actor in World (needs World->Scene) and collects the hidden actors. */
		bool Begin(UWorld* World, const FDepthCaptureSettings& Settings, FString& OutError);

		/**
		 * Captures the camera's pose (HarmonyTraining::ColmapCameraToUnrealPose) at Camera.Width x Camera.Height.
		 * Captures up to 1 + SettleMax times and stops once two consecutive reads are bitwise equal (Nanite / texture
		 * streaming settle); bOutSettled = false when they never were (the last read is returned). SettleMax 0: one capture.
		 * OutDepthCm: Width*Height planar Z in cm, row 0 = top (the target's R channel).
		 */
		bool Capture(const FHarmonyTrainingCamera& Camera, int32 SettleMax, TArray<float>& OutDepthCm, int32& OutNumCaptures,
			bool& bOutSettled, FString& OutError);

		/** Destroys the capture actor and releases the target. Called by the destructor too. */
		void End();

		int32 GetNumHiddenActors() const { return NumHiddenActors; }

	private:
		bool EnsureTarget(int32 Width, int32 Height, FString& OutError);
		bool CaptureOnce(TArray<float>& OutDepthCm, FString& OutError);

		TWeakObjectPtr<UWorld> World;
		TWeakObjectPtr<ASceneCapture2D> CaptureActor;
		TStrongObjectPtr<UTextureRenderTarget2D> Target;
		FDepthCaptureSettings Settings;
		TArray<FLinearColor> Readback;
		int32 NumHiddenActors = 0;
	};

	/**
	 * The P2 tool (UHarmonyTrainingEditorDepthLibrary::BuildDepthSeedInit):
	 *  1. LoadColmapDataset(SparseDir, ImagesDir, eval_interval) for the training images (seed colours = images_2 pixels)
	 *     and the train/val split; LoadColmapText(SparseDir, 1) for the full-resolution cameras (depth intrinsics).
	 *  2. Loads MapPath into the editor world (empty: use the current editor world), checks every streaming level is
	 *     loaded and visible, finishes asset and shader compilation.
	 *  3. One FHarmonyDepthCapture per seeded view (train views, or all with seed_views "all") -> FDepthSeedAccumulator.
	 *     verify_views views are also checked against <SparseDir>/points3D (Rotor depth samples) as a pose / FOV test.
	 *  4. Finalize -> BuildInitFromSeeds -> WriteStateDir(OutStateDir) (TrainSplat's Init), plus
	 *     <OutStateDir>/seeds/{points3D,cameras,images}.txt (a COLMAP model for the "colmap" init) and
	 *     <OutStateDir>/depth_init.json (options, statistics, verification).
	 * Options: HarmonyTraining::ParseDepthSeedOptions. Returns "OK: ..." or "ERROR: ...". Game thread, blocking.
	 */
	HARMONYTRAININGEDITOR_API FString BuildDepthSeedInit(const FString& MapPath, const FString& SparseDir, const FString& ImagesDir,
		const FString& OutStateDir, const FString& OptionsJson);
}
