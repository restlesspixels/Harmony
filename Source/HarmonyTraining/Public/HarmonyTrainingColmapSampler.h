// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * The COLMAP points3D sampler of RotorSplatRenderTools' payload generator (ColmapRenderPayloadGenerator.cpp,
 * GeneratePayloadFromCameraProvider and helpers), ported line by line under new names so Harmony's MRQ capture builds the
 * same initial point cloud without depending on Rotor (notes/impl_p4_sampler.md). CPU only, no editor dependency.
 *
 * Identity contract: for the same frames (same order, poses, intrinsics), the same 8-bit sRGB colour and the same R32F depth
 * replays, FColmapPointSampler emits the same points (count, order, double XYZ, RGB, tracks) and the same POINTS2D as Rotor,
 * so WriteColmapSamplerPoints3D / FormatColmapSamplerPoints2DLine give byte-identical text. Arithmetic, iteration order,
 * hashing ("RNG": GetTypeHash / HashCombine of frame number + seed and tile), Algo::Sort comparators and defaults are
 * Rotor's; settings defaults are those of Rotor's MRQ output (UColmapRenderMoviePipelineOutput::MakePayloadSettings with the
 * sampling policy of r.Rotor.ColmapPreview.* at their defaults, MaskType None).
 *
 * Streaming: Rotor samples frame by frame (each frame needs only its own colour + depth; all cross-frame state is in the
 * bucket accumulators), so frames can be fed one at a time. The colour enters only through ComputeRGBDetailScore and the
 * colour of the sampled pixel, at pixel positions that depend on the frame number and image size only, so
 * BuildColmapSamplerColor reduces a full image to those values (about 200 KB per 1024^2 frame instead of 4 MiB) as soon as
 * it is rendered; AddFrame with that summary is the same computation as AddFrame with the full image.
 */
namespace HarmonyTraining
{
	/** Rotor FColmapRenderPayloadSettings as Rotor's MRQ output fills it (MakePayloadSettings), plus the sampling policy. */
	struct FColmapSamplerSettings
	{
		int32 TargetPoints = 500000;
		int32 TileSizePx = 16;
		int32 MinTrackLength = 1;
		/** Rotor's MRQ output passes bMaskActive (false for MaskType None); the payload default would be true. */
		bool bUseImageAlphaAsMask = false;
		double MaxDepthMeters = 10000.0;
		double MaxTrackSourceOffsetPx = 1.0;
		double SpatialBalanceNearCellMeters = 1.0;
		double SpatialBalanceMidStartMeters = 60.0;
		double SpatialBalanceMidCellMeters = 1.0;
		double SpatialBalanceFarStartMeters = 200.0;
		double SpatialBalanceFarCellMeters = 12.0;
		bool bUseDistantBackgroundCull = false;
		double DistantBackgroundStartMeters = 200.0;
		int32 DistantBackgroundMaxSelectedPoints = 0;
		bool bUseOriginCull = false;
		double OriginCullRadiusMeters = 0.0;
		FVector3d OriginCullCenterMeters = FVector3d::ZeroVector;
		FString OriginCullShape = TEXT("SPHERE");
		double OriginCullMinZOffsetMeters = -1.0;
		double OriginCullMaxZOffsetMeters = 2.5;
		bool bUseDepthBalancedSelection = true;
		double DepthBalanceNearMeters = 15.0;
		double DepthBalanceFarMeters = 60.0;
		double DepthBalanceNearWeight = 1.0;
		double DepthBalanceMidWeight = 1.0;
		double DepthBalanceFarWeight = 10.0;
		int32 FrameCoverageMinPrimaryPoints = 32;

		/** Rotor::ColmapBufferVisualizations::FSamplingPolicy (r.Rotor.ColmapPreview.* defaults; float as there, the clamps of
		    GetSamplingPolicy are applied when resolving). */
		float PolicyRGBWeight = 0.65f;
		float PolicyNormalWeight = 0.25f;
		float PolicyDepthWeight = 0.10f;
		float PolicyRGBThreshold = 0.12f;
		float PolicyNormalThreshold = 0.18f;
		float PolicyDepthThreshold = 0.015f;
		float PolicyCombinedThreshold = 0.25f;
		float PolicyHighThreshold = 0.50f;
		float PolicyVeryHighThreshold = 0.75f;
		float PolicyBandPassWeight = 0.35f;
		float PolicyFineTextureWeight = 1.0f;
		int32 PolicyCoverageTileSizePx = 64;
		int32 PolicyDetailTileSizePx = 16;
		int32 PolicySeed = 1337;
	};

	/** One captured frame (Rotor FColmapRenderCapturedFrame without pixels). Vectors as recorded (AddFrame normalises them
	    again, as Rotor's FInputImage copy does). Width / Height = the FinalImage (and depth) size. */
	struct FColmapSamplerFrame
	{
		FString ImageName;
		int32 FrameNumber = 0;   // Rotor FInputImage::FrameNumber = the captured SourceFrameNumber
		int32 Width = 0;
		int32 Height = 0;
		FVector3d CameraLocationCm = FVector3d::ZeroVector;
		FVector3d CameraRightUE = FVector3d::UnitY();
		FVector3d CameraUpUE = FVector3d::UnitZ();
		FVector3d CameraForwardUE = FVector3d::UnitX();
		double FocalLengthMm = 0.0;
		double SensorWidthMm = 0.0;
		double SensorHeightMm = 0.0;
	};

	/** What the sampler needs from one frame's colour: the RGB detail score and the BGRA colour at every candidate pixel
	    (coverage tiles row-major; detail tiles row-major x 4 sub-samples). */
	struct FColmapSamplerColor
	{
		int32 FrameNumber = 0;
		int32 Width = 0;
		int32 Height = 0;
		TArray<double> CoverageScore;
		TArray<FColor> CoverageColor;
		TArray<double> DetailScore;
		TArray<FColor> DetailColor;

		int64 GetAllocatedSize() const
		{
			return CoverageScore.GetAllocatedSize() + CoverageColor.GetAllocatedSize() + DetailScore.GetAllocatedSize() + DetailColor.GetAllocatedSize();
		}
	};

	struct FColmapSamplerTrack
	{
		int32 ImageId = INDEX_NONE;
		int32 Point2DIndex = INDEX_NONE;
	};

	/** Rotor FOutputPoint: COLMAP world metres (X = UE X, Y = -UE Z, Z = -UE Y), the first sample's colour, the track. */
	struct FColmapSamplerPoint
	{
		FVector3d PositionMeters = FVector3d::ZeroVector;
		FColor Color = FColor::White;
		TArray<FColmapSamplerTrack> Tracks;
	};

	/** Rotor FImagePoint2D. */
	struct FColmapSamplerPoint2D
	{
		float X = 0.0f;
		float Y = 0.0f;
		int32 Point3DId = INDEX_NONE;
	};

	struct FColmapSamplerOutput
	{
		TArray<FColmapSamplerPoint> Points;                     // POINT3D_ID = index + 1
		TArray<TArray<FColmapSamplerPoint2D>> Points2DByFrame;  // IMAGE_ID = frame index (AddFrame order)
		/** Rotor's validation_report.txt body (statistics and resolved settings; ImageDir / OutputDir / Images lines aside). */
		FString Report;
	};

	/** Rotor's ColmapRenderPayload sampler. Not thread-safe; AddFrame calls must be sequential (any thread). */
	class HARMONYTRAINING_API FColmapPointSampler
	{
	public:
		explicit FColmapPointSampler(const FColmapSamplerSettings& Settings);
		~FColmapPointSampler();
		FColmapPointSampler(const FColmapPointSampler&) = delete;
		FColmapPointSampler& operator=(const FColmapPointSampler&) = delete;

		/** False (with the reason) when the settings are invalid (Rotor MakeGeneratorSettings checks). */
		bool IsValid(FString* OutError = nullptr) const;

		/** Rotor's per-frame loop body (Generator.cpp:2536-2854) for the next frame. Frames must come in Rotor's order:
		    ascending (FrameNumber, ImageName with FString operator<); an out-of-order frame is an error. DepthCm is the R
		    channel of the R32F SceneDepth replay, Width x Height. */
		bool AddFrame(const FColmapSamplerFrame& Frame, const FColmapSamplerColor& Color, TArrayView<const float> DepthCm, FString& OutError);
		/** Same, from the full 8-bit sRGB BGRA image (what Rotor re-reads from its PNG). */
		bool AddFrame(const FColmapSamplerFrame& Frame, TArrayView64<const FColor> Pixels, TArrayView<const float> DepthCm, FString& OutError);

		/** Selection + tracks (Generator.cpp:2856-3031). Call once after the last AddFrame. */
		bool Finalize(FColmapSamplerOutput& Out, FString& OutError);

		int32 NumFrames() const;
		int32 NumBuckets() const;
		/** One-line summary of the run statistics so far. */
		FString GetStatsString() const;

		/** Reduces one frame's full image (Width x Height BGRA8 sRGB) to the candidates' RGB scores and colours. Any thread. */
		static bool BuildColor(const FColmapSamplerSettings& Settings, int32 FrameNumber, TArrayView64<const FColor> Pixels, int32 Width, int32 Height,
			FColmapSamplerColor& Out, FString& OutError);

	private:
		struct FImpl;
		TUniquePtr<FImpl> Impl;
	};

	/** Rotor FormatDouble: "%.9g". */
	HARMONYTRAINING_API FString FormatColmapSamplerDouble(double Value);

	/** Rotor WritePoints3DFile (header, "%d %.9g %.9g %.9g %d %d %d 0" + " IMAGE_ID POINT2D_IDX" per track), UTF-8,
	    LINE_TERMINATOR. Creates the directory. */
	HARMONYTRAINING_API bool WriteColmapSamplerPoints3D(const FString& Path, const TArray<FColmapSamplerPoint>& Points, FString& OutError);

	/** Rotor WriteImagesFile's POINTS2D line for one image ("%.9g %.9g %d" joined by spaces; empty for none). */
	HARMONYTRAINING_API FString FormatColmapSamplerPoints2DLine(const TArray<FColmapSamplerPoint2D>& Points);

	/** Rotor WriteImagesFile's header value: total observations / images, "%.9g". */
	HARMONYTRAINING_API FString FormatColmapSamplerMeanObservations(const TArray<TArray<FColmapSamplerPoint2D>>& Points2DByFrame);

	/** The Init "colmap" keys of a TrainSplat OptionsJson, parsed as TrainSplat parses them (HarmonyTrainingTrainerCore.cpp
	    ParseTrainOptions: init_scale / init_opacity numbers cast to float, rot_seed a number <= 2^53 or a decimal / 0x-hex
	    string); other keys are ignored. Defaults 0.1 / 0.5 / 42. */
	HARMONYTRAINING_API bool ParseColmapInitOptions(const FString& TrainOptionsJson, float& OutInitScale, float& OutInitOpacity, uint64& OutRotSeed,
		FString& OutError);

	/** Exactly TrainSplat's Init "colmap" (LoadColmapPoints(SparseDir) + BuildInitFromPoints with ParseColmapInitOptions of
	    TrainOptionsJson), written as a state dir (WriteStateDir) so a session can start from it. */
	HARMONYTRAINING_API bool BuildColmapInitStateDir(const FString& SparseDir, const FString& StateDir, const FString& TrainOptionsJson, int32& OutNumSplats,
		FString& OutError);
}
