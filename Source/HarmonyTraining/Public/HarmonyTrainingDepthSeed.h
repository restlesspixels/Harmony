// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/CriticalSection.h"
#include "HarmonyTrainingTypes.h"

/**
 * Phase 4 P2: depth-seeded init (CONTRACT_P4.md section 3, notes/phase4_depth.md sections 2-4 and 7, phase4_critic.md).
 * CPU only, no world, any thread. The world side (pose replay with a SceneCapture2D, SCS_SceneDepth R32F) lives in
 * HarmonyTrainingEditor (HarmonyTrainingDepthCapture.h); the Phase 4 MRQ output streams its views into the same
 * FDepthSeedAccumulator, so there is one seeding algorithm.
 *
 * Frames: the trainer frame is COLMAP world in metres, COLMAP = 0.01 * (UE_X, -UE_Z, -UE_Y). Depth is SCS_SceneDepth:
 * planar view-space Z in cm (not the ray length), row 0 = top. Pixel (i, j) covers [i, i+1) x [j, j+1) and has its
 * centre at (i + 0.5, j + 0.5); the trainer's HTProjectForward gives PixelXy = (U - 0.5, V - 0.5) in this convention.
 *
 * Not implemented in Phase 4 (CONTRACT_P4 section 3 decisions): glass-surface seeds (behind-glass only, which is what an
 * opaque depth pass sees), the sky shell, median colour and normal-oriented splats. FHarmonyTrainingSeedPoints::Class is
 * kept (always 0 = surface) so those can be added without an API change.
 */
namespace HarmonyTraining
{
	/** Sampling / dedupe settings (phase4_depth.md section 3). */
	struct FHarmonyTrainingDepthInitSettings
	{
		/** Output budget: the voxel size is raised until N <= TargetPoints (N may end below it; MCMC grows to cap_max). */
		int32 TargetPoints = 500000;
		/** One jittered candidate per TileSizePx x TileSizePx tile per view (section 3.2). */
		int32 TileSizePx = 12;
		/** Jittered pixels tried per tile before giving up (1 + 3 retries). 1..8. */
		int32 MaxTriesPerTile = 4;
		/** Jitter seed: pixel = f(InitMix64(Seed, view, tile, try)); independent of thread and view order. */
		uint64 Seed = 42;
		/** Depth validity (cm): MinDepthCm < Z < MaxDepthCm; Z >= MaxDepthCm (or +inf) is sky; NaN is invalid. */
		float MinDepthCm = 1.0f;
		float MaxDepthCm = 1.0e6f;
		/** Depth-edge rejection: max3x3(Z) > min3x3(Z) * (1 + EdgeRatio), or any 3x3 neighbour invalid / sky. */
		float EdgeRatio = 0.03f;
		/** Grazing rejection: |n . ray| < MinCosView, n from the depth neighbours (central differences). */
		float MinCosView = 0.1f;
		/** Footprint f = (z / fx) / max(|n . ray|, MinFootprintCos), metres per pixel at the depth resolution. */
		float MinFootprintCos = 0.25f;
		/** Initial cell = Alpha * footprint (level L = floor(log2(Alpha * f / MinCellM)), cell MinCellM * 2^L). <= 0: TileSizePx. */
		float Alpha = 12.0f;
		/** Cell of level 0 (metres). Levels are clamped to [0, 40]; there is no upper clamp on the cell otherwise. */
		double MinCellM = 0.001;
		/** Drop a representative whose cell contains a kept finer representative (section 3.3, finalize). */
		bool bCoarseSuppression = true;
		/** Scale post-pass of BuildInitFromSeeds: s = clamp(s, ScaleClampMin * cell, ScaleClampMax * cell). */
		bool bScaleClamp = true;
		float ScaleClampMin = 0.02f;
		float ScaleClampMax = 0.5f;
	};

	/** One view handed to the accumulator. Every array is borrowed for the duration of AddView only. */
	struct FHarmonyTrainingDepthView
	{
		/** COLMAP camera with intrinsics AT THE DEPTH RESOLUTION (Width x Height = DepthCm size), e.g. LoadColmapText(.., 1). */
		const FHarmonyTrainingCamera* Camera = nullptr;
		/** Width*Height planar view Z in cm (SCS_SceneDepth, R32F), row-major, row 0 = top. */
		TConstArrayView<float> DepthCm;
		/** The training image of this view: (Width/ColorDownscale) x (Height/ColorDownscale) RGBA8 in a uint32, R in the low
		    byte (FHarmonyTrainingDataset::Images layout). Depth pixel (x, y) takes colour pixel (x/ds, y/ds), clamped. */
		TConstArrayView<uint32> Color;
		int32 ColorDownscale = 1;
		/** Stable view id (the name-sorted camera index): keys the jitter and breaks ties. 0..2^31-1. */
		int32 ViewIndex = 0;
	};

	/** Seeds in the trainer frame. Parallel arrays. */
	struct FHarmonyTrainingSeedPoints
	{
		TArray<FVector3f> Xyz;   // COLMAP world, metres
		TArray<FColor> Rgb;      // 8-bit sRGB (R, G, B fields; A = 255), the images_2 pixel
		TArray<float> CellM;     // final voxel size of the seed (metres)
		TArray<uint8> Class;     // 0 surface (1 glass surface, 2 sky shell: reserved, not produced in Phase 4)

		int32 Num() const { return Xyz.Num(); }
		void Reset()
		{
			Xyz.Reset();
			Rgb.Reset();
			CellM.Reset();
			Class.Reset();
		}
	};

	/** Counters of an accumulator (cumulative over AddView; the finalize fields are set by Finalize). */
	struct FHarmonyTrainingDepthSeedStats
	{
		int64 Views = 0;
		int64 Tiles = 0;           // tiles visited
		int64 TilesSeeded = 0;     // tiles that produced a candidate
		int64 Tries = 0;           // jittered pixels evaluated
		int64 Invalid = 0;         // NaN, <= MinDepthCm, or a degenerate neighbourhood
		int64 Sky = 0;             // >= MaxDepthCm
		int64 Edge = 0;            // depth edge or invalid / sky neighbour
		int64 Grazing = 0;         // |n . ray| < MinCosView
		int64 Candidates = 0;      // accepted candidates (= TilesSeeded)
		int32 Compactions = 0;     // in-flight dedupes (memory bound)
		double AddSeconds = 0.0;

		int32 Representatives = 0; // unique cells at the initial Alpha
		int32 Suppressed = 0;      // coarse representatives dropped at the final Alpha
		int32 Output = 0;
		float FinalAlpha = 0.0f;
		int32 AlphaPasses = 0;
		int32 MinLevel = 0, MaxLevel = 0;
		double FinalizeSeconds = 0.0;

		/** "views=.. tiles=.. ..." (one line, for result strings). */
		HARMONYTRAINING_API FString ToString() const;
	};

	/**
	 * Per-view stratified candidates + footprint-tied multi-level voxel dedupe (phase4_depth.md section 3).
	 * AddView may be called from any thread and concurrently (one lock around the merge); views can arrive in any order:
	 * the result depends only on the SET of views (ties break on (footprint, view, pixel)). Memory: about 28 bytes per
	 * kept candidate; candidates are deduped in place whenever the store doubles (the dedupe is idempotent, so this does
	 * not change the result).
	 */
	class HARMONYTRAINING_API FDepthSeedAccumulator
	{
	public:
		/** One accepted depth sample (internal; public so the implementation's helpers can name it). */
		struct FCandidate
		{
			float P[3];      // COLMAP world, metres
			float F;         // footprint, metres per pixel (already divided by max(cos, MinFootprintCos))
			int32 View;
			uint32 Pixel;    // y * Width + x at the depth resolution
			uint32 Rgba;     // colour, R in the low byte
		};

		explicit FDepthSeedAccumulator(const FHarmonyTrainingDepthInitSettings& InSettings);

		/** Samples one view (ParallelFor over tile rows inside). False (and nothing added) on bad sizes / camera. */
		bool AddView(const FHarmonyTrainingDepthView& View, FString& OutError);

		/** Dedupe, coarse suppression, budget search, deterministic order (L, Morton(cell)). Call once, after every AddView. */
		bool Finalize(FHarmonyTrainingSeedPoints& Out, FString& OutError);

		const FHarmonyTrainingDepthSeedStats& GetStats() const { return Stats; }
		const FHarmonyTrainingDepthInitSettings& GetSettings() const { return Settings; }

	private:
		FHarmonyTrainingDepthInitSettings Settings;
		double Alpha0 = 12.0;
		FCriticalSection Lock;
		TArray<FCandidate> Candidates;
		int32 CompactAt = 0;
		FHarmonyTrainingDepthSeedStats Stats;
		bool bFinalized = false;
	};

	/**
	 * BuildInitFromPoints (unchanged: KNN scale, seeded rotations, SH DC) + the section 4 post-pass: when
	 * Settings.bScaleClamp, each (isotropic, activated) scale is clamped to [ScaleClampMin, ScaleClampMax] * CellM[i].
	 * Opacity stays InitOpacity (only surface seeds exist in Phase 4). OutNumClamped (optional): seeds whose scale changed.
	 */
	HARMONYTRAINING_API bool BuildInitFromSeeds(const FHarmonyTrainingSeedPoints& Seeds, float InitScale, float InitOpacity,
		uint64 RotSeed, const FHarmonyTrainingDepthInitSettings& Settings, FHarmonyTrainingSplatsCPU& Out, FString& OutError,
		int32* OutNumClampedLow = nullptr, int32* OutNumClampedHigh = nullptr);

	/** Debug / A-B: writes a COLMAP points3D.txt ("id x y z r g b 0", no tracks, %.9g so floats round-trip) that the
	    existing LoadColmapPoints / "colmap" init reads unchanged. Creates the directory. */
	HARMONYTRAINING_API bool WriteSeedPointsText(const FString& Path, const FHarmonyTrainingSeedPoints& Seeds, FString& OutError);

	// ---- Geometry (exported for the collector and the unit test) ----

	/** Camera-space (COLMAP: x right, y down, z forward) point of continuous pixel (U, V) at planar depth DepthCm, metres. */
	HARMONYTRAINING_API FVector3d DepthPixelToCamera(const FHarmonyTrainingCamera& Camera, double U, double V, double DepthCm);

	/** World (COLMAP, metres) point of continuous pixel (U, V) at planar depth DepthCm: p_w = R^-1 (p_c - t) in double (the exact
	    inverse of the float matrix the trainer projects with; R^T for an exact rotation). */
	HARMONYTRAINING_API FVector3d DepthPixelToWorld(const FHarmonyTrainingCamera& Camera, double U, double V, double DepthCm);

	/** Inverse of DepthPixelToWorld in double: (U, V) = (fx x/z + cx, fy y/z + cy), OutZ = camera z (metres). */
	HARMONYTRAINING_API void ProjectWorldToPixel(const FHarmonyTrainingCamera& Camera, const FVector3d& World, double& OutU, double& OutV, double& OutZ);

	/** COLMAP world-to-camera of a camera -> Unreal pose (for a SceneCapture2D replay). */
	struct FHarmonyTrainingUnrealPose
	{
		FVector3d LocationCm = FVector3d::ZeroVector;   // UE world, cm
		FVector3d Forward = FVector3d::ForwardVector;   // UE X axis of the camera
		FVector3d Right = FVector3d::RightVector;       // UE Y axis
		FVector3d Up = FVector3d::UpVector;             // UE Z axis
		double HorizontalFovDeg = 90.0;                 // 2 atan(W / (2 fx)); USceneCaptureComponent2D::FOVAngle (fy = fx)
	};

	/**
	 * Rows of R are (right, -up, forward) in COLMAP world (Rotor BuildWorldToColmapCamera); UE = 100 * (x, -z, -y) for
	 * points and (x, -z, -y) for directions; C = -R^T t. Fails when the intrinsics cannot be reproduced by a centred,
	 * square-pixel perspective capture: |fx - fy| > 1e-5 fx, |cx - W/2| > 1e-3 or |cy - H/2| > 1e-3 px.
	 */
	HARMONYTRAINING_API bool ColmapCameraToUnrealPose(const FHarmonyTrainingCamera& Camera, FHarmonyTrainingUnrealPose& Out, FString& OutError);

	// ---- Options of the depth-init tool (UHarmonyTrainingEditorDepthLibrary::BuildDepthSeedInit) ----

	/** Everything BuildDepthSeedInit takes from OptionsJson. Parsed here so the editor module needs no Json dependency. */
	struct FHarmonyTrainingDepthSeedOptions
	{
		FHarmonyTrainingDepthInitSettings Settings;
		// init (BuildInitFromSeeds)
		float InitScale = 0.1f;
		float InitOpacity = 0.5f;
		uint64 RotSeed = 42;
		// views
		bool bTrainViewsOnly = true;   // "seed_views": "train" | "all"
		int32 EvalInterval = 8;
		int32 MaxViews = 0;            // > 0: evenly spaced subset (smoke runs)
		// capture
		int32 SettleMax = 4;           // re-captures per view until two reads are bitwise equal (streaming); 0 = one capture
		int32 FirstSettleMax = 32;     // the same for the first view
		TArray<FString> HideActorClasses = { TEXT("SpatialCaptureRig") };   // class-name substrings (incl. super classes)
		// diagnostics / outputs
		int32 VerifyViews = 8;         // views checked against <SparseDir>/points3D (pose / FOV / depth consistency); 0 = off
		FString DepthDumpDir;          // "" = off; else <dir>/<image base>.f32 at the colour resolution (min of each ds x ds block)
		int32 DepthDumpEvery = 13;     // dump every Nth seeded view (by sorted camera index)
		FString PointsTxt;             // "" = <OutStateDir>/seeds/points3D.txt; "-" = do not write
	};

	/**
	 * Keys (all optional): target_points, tile_px, max_tries, seed, min_depth_cm, max_depth_cm, edge_ratio, min_cos_view,
	 * min_footprint_cos, alpha, min_cell_m, coarse_suppression, scale_clamp, scale_clamp_min, scale_clamp_max, init_scale,
	 * init_opacity, rot_seed (number or decimal string), seed_views ("train"|"all"), eval_interval, max_views, settle_max,
	 * first_settle_max, hide_actor_classes (array of strings), verify_views, depth_dump_dir, depth_dump_every, points_txt.
	 * Unknown keys are logged as warnings. Empty / whitespace JSON keeps InOut.
	 */
	HARMONYTRAINING_API bool ParseDepthSeedOptions(const FString& OptionsJson, FHarmonyTrainingDepthSeedOptions& InOut, FString& OutError);
}
