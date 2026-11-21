// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HarmonyTrainingTypes.h"   // FHarmonyTrainingSplatsCPU
#include "Tasks/Task.h"

struct FHarmonyRawData;
class UHarmonyDataAsset;

/**
 * HarmonyTraining Phase 4, piece P1 (CONTRACT_P4.md section 2): trained splats -> Harmony asset.
 * WriteHarmonyAsset and CommitHarmonyAsset are game thread only (UObject creation, package save); the data half
 * (BuildHarmonyRawData) runs on any thread, which FHarmonyAssetWriteJob uses for a non-blocking save.
 */
namespace HarmonyTrainingEditor
{
	/** What WriteHarmonyAsset did (optional out). */
	struct FHarmonyAssetWriteResult
	{
		FString ObjectPath;       // /Pkg/Path/Name.Name
		int32 NumPoints = 0;      // splats in the saved asset (after dropping non-finite rows)
		int32 NumDropped = 0;     // non-finite rows dropped
		bool bOverwrote = false;  // an existing UHarmonyDataAsset (loaded or on disk) was overwritten in place
		bool bCompressed = false; // saved with EHarmonyAssetEncoding::Compressed
	};

	/**
	 * Splats in the trainer's device layout -> Expanded FHarmonyRawData, equal to what UHarmonyPlyFactory produces when the
	 * same splats are written with HarmonyTraining::WritePly and imported (before the factory's optional compression):
	 * - position: FHarmonySplatOps::TransformPosition(xyz, WebSplatToUnrealFrame, 100) = 100 * (x, -z, -y), w = 1;
	 * - covariance: FHarmonySplatOps::BuildCovarianceFromRawPly(log(scale), rot (w,x,y,z), frame, 100), with log(scale) the
	 *   value WritePly stores (the importer exponentiates it again; vs. a PLY not written from these splats the relative
	 *   difference is the exp/log round trip, < 1e-6);
	 * - opacity: Color = (0, 0, 0, clamp(sigmoid(logit(o)), 0, 1)) with WritePly's logit and the importer's sigmoid;
	 * - SH: un-reordered, SHCoeffs[16 i + k] = (r, g, b, 0), k = 0 the DC term (raw, no rotation);
	 * - NumPoints, SHDegree 3, UnitScale 100, Expanded, Gaussian3D, BoundsMin/Max over the source xyz (COLMAP metres),
	 *   UpdateDebugStats(). SourceFilename is left empty.
	 * Every row is converted, finite or not (the factory keeps them all too). Fails only on inconsistent input arrays.
	 */
	HARMONYTRAININGEDITOR_API bool ConvertToHarmonyRawData(const FHarmonyTrainingSplatsCPU& Splats, FHarmonyRawData& Out, FString& OutError);

	/**
	 * Drops every row of an Expanded FHarmonyRawData with a non-finite position, opacity, covariance or SH value, keeping
	 * the order of the rest; updates NumPoints and the debug stats (bounds are not touched). Returns the number of rows
	 * dropped, or -1 (and changes nothing) when Data is not a consistent Expanded Gaussian3D payload.
	 */
	HARMONYTRAININGEDITOR_API int32 RemoveNonFiniteSplats(FHarmonyRawData& Data);

	/**
	 * Converts Splats (ConvertToHarmonyRawData), drops non-finite rows (also rows whose xyz / rotation / scale / opacity is
	 * non-finite in Splats; the count is logged and returned in OutResult), optionally compresses with Harmony's own
	 * Harmony::Editor::CompressDataAssetInPlace (as the PLY factory does), and saves the package.
	 * - LongPackageName: e.g. /Env_Showroom/Splats/Day_orbit; the asset is named after the last path element.
	 * - An existing UHarmonyDataAsset at that path (in memory or on disk) is loaded and overwritten in place, and Harmony's
	 *   render resources are released so components showing it re-upload. Anything else at that path is an error (never
	 *   replaced). A read-only package file (not checked out) is an error, reported before anything is changed.
	 * - The new data is built and compressed on a transient object first, so a failure leaves an existing asset untouched.
	 * - SourceFilename is stored in FHarmonyRawData::SourceFilename (empty: "HarmonyTraining").
	 * - bAntialiased (training option antialiased) sets UHarmonyDataAsset::bTrainedAntialiased.
	 * Returns the saved asset, or nullptr with OutError set.
	 */
	HARMONYTRAININGEDITOR_API UHarmonyDataAsset* WriteHarmonyAsset(const FString& LongPackageName, const FHarmonyTrainingSplatsCPU& Splats,
		bool bCompress, FString& OutError, const FString& SourceFilename = FString(), FHarmonyAssetWriteResult* OutResult = nullptr,
		bool bAntialiased = false);

	/**
	 * Phase 9: the inverse of ConvertToHarmonyRawData, so refinement can start from a Harmony asset (shared through
	 * source control) without any extra file. A compressed asset is decompressed on a transient copy (lossy: the
	 * quantisation is what fine-tuning then trains away). Position = (P.X, -P.Z, -P.Y) / 100; covariance
	 * F Sigma F / 100^2 (F the WebSplat <-> UE axis swap) -> eigen decomposition -> scale = sqrt(lambda), rotation =
	 * eigenvectors (right-handed); opacity = Color.W; SH copied back (16 coefficients, zero-padded).
	 */
	HARMONYTRAININGEDITOR_API bool ConvertHarmonyAssetToSplats(const UHarmonyDataAsset& Asset, FHarmonyTrainingSplatsCPU& Out, FString& OutError);

	// ---- The two halves of WriteHarmonyAsset (Phase 5: the editor's save no longer blocks the game thread) ----

	/**
	 * WriteHarmonyAsset steps 2-4 on plain data, any thread: convert, drop non-finite rows (OutNumDropped), store
	 * SourceFilename, and compress with Harmony::Editor::CompressSplatDataInPlace when bCompress. No UObject is touched.
	 */
	HARMONYTRAININGEDITOR_API bool BuildHarmonyRawData(const FHarmonyTrainingSplatsCPU& Splats, bool bCompress, const FString& SourceFilename,
		FHarmonyRawData& OutData, int32& OutNumDropped, FString& OutError);

	/** WriteHarmonyAsset steps 1 and 5-7, game thread: resolve the target, move Data into it, save, release render resources. */
	HARMONYTRAININGEDITOR_API UHarmonyDataAsset* CommitHarmonyAsset(const FString& LongPackageName, FHarmonyRawData&& Data, int32 NumDropped,
		FString& OutError, FHarmonyAssetWriteResult* OutResult = nullptr);

	/**
	 * WriteHarmonyAsset with the build on a worker (UE::Tasks). Start() checks the target on the game thread (same errors
	 * as WriteHarmonyAsset, before any work) and launches the build; poll IsBuildDone() from a tick and call Commit() on the
	 * game thread (it waits if the build is still running). The result equals WriteHarmonyAsset's bit for bit. The splats
	 * are shared, not copied, and must not change until the build is done. Destroying the job waits for the worker.
	 */
	class HARMONYTRAININGEDITOR_API FHarmonyAssetWriteJob
	{
	public:
		static TSharedPtr<FHarmonyAssetWriteJob> Start(const FString& LongPackageName, const TSharedRef<const FHarmonyTrainingSplatsCPU>& Splats,
			bool bCompress, const FString& SourceFilename, FString& OutError, bool bAntialiased = false);
		~FHarmonyAssetWriteJob();

		bool IsBuildDone() const;
		void WaitForBuild();
		/** Game thread, once: commit and save. nullptr + OutError on a build or commit error. */
		UHarmonyDataAsset* Commit(FString& OutError, FHarmonyAssetWriteResult* OutResult = nullptr);
		const FString& GetLongPackageName() const { return LongPackageName; }

	private:
		FHarmonyAssetWriteJob();
		struct FBuild;
		FString LongPackageName;
		TSharedPtr<FBuild> Build;
		UE::Tasks::FTask Task;
		double StartSeconds = 0.0;
	};
}
