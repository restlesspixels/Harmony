// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "Templates/SharedPointer.h"

#include "HarmonyTrainingEditorSubsystem.generated.h"

class AActor;
class FHarmonyExternalSplatSource;
class FHarmonyTrainingEditorTicker;
class FHarmonyTrainingSession;
class UHarmonyDataAsset;
class UWorld;
struct FHarmonyTrainingDataset;
namespace HarmonyTrainingEditor { class FHarmonyAssetWriteJob; }

/** What to train (UHarmonyTrainingEditorSubsystem::StartTraining). The training fields mirror UHarmonyTrainingLibrary::TrainSplat. */
USTRUCT(BlueprintType)
struct HARMONYTRAININGEDITOR_API FHarmonyTrainingRequest
{
	GENERATED_BODY()

	/** Init splats: "colmap" (points3D of ColmapSparseDir), a state dir or a PLY. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmony|Training")
	FString Init = TEXT("colmap");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmony|Training")
	FString ColmapSparseDir;

	/** Images folder named as in images.txt; the downscale comes from a trailing "_<n>" (images_2 -> 2). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmony|Training")
	FString ImagesDir;

	/** 0 = the project default (Project Settings > Plugins > Harmony Training, DefaultTrainingSteps). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmony|Training")
	int32 Steps = 0;

	/** TrainSplat's OptionsJson keys (HarmonyTrainingLibrary.h), merged per key over the project's
	    DefaultTrainingOptionsJson (default {"strategy":"mcmc"}). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmony|Training")
	FString OptionsJson;

	/** Optional PLY (+ <base>_train.json / _loss.f32). Empty: no files. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmony|Training")
	FString OutPly;

	/** Optional long package name (e.g. /Env_Showroom/Splats/SPL_Day): saved with the P1 writer when training is done. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmony|Training")
	FString AssetPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmony|Training")
	bool bCompressAsset = true;

	/** Editor frame budget, training included (ms). 0 = turbo. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmony|Training")
	float FrameBudgetMs = 33.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmony|Training")
	int32 MinStepsPerTick = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmony|Training")
	int32 MaxStepsPerTick = 64;

	/** Draw the training splats live with Harmony's renderer in the editor world (zero-copy GPU buffers). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmony|Training")
	bool bLivePreview = true;

	/** Minimum seconds between two preview refreshes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmony|Training")
	float PreviewIntervalSeconds = 0.1f;

	/** Pause while Play-In-Editor runs (including the MRQ PIE executor), resume afterwards. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmony|Training")
	bool bPauseDuringPIE = true;

	/** Keep the editor at full frame rate in the background while training (no 3 fps CPU throttling). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmony|Training")
	bool bKeepTrainingInBackground = true;
};

/** Snapshot of the running (or last) session (UHarmonyTrainingEditorSubsystem::GetStatus). */
USTRUCT(BlueprintType)
struct HARMONYTRAININGEDITOR_API FHarmonyTrainingStatus
{
	GENERATED_BODY()

	/** Idle, Loading, Uploading, Training, Paused, Draining, Finalizing, Saving (Request.AssetPath), Done, Failed, Cancelled. */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") FString State = TEXT("Idle");
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 Step = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 Steps = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 EnqueuedSteps = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 NumSplats = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 StateCapacity = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") float Loss = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") float Psnr = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") float MsPerStep = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 StepsPerTick = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") float GpuFrameMs = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 OverflowSteps = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 EvalDone = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 EvalTotal = 0;
	/** Val PSNR (Done, eval on). */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") float ValPsnrMean = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") float ValPsnrMin = 0.0f;

	/** Editor frames while Training (not paused) since the start or ResetFrameStats: wall frame time and the
	    game thread's own time (GGameThreadTime), ms. */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 NumFrames = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") float FrameMsP50 = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") float FrameMsP95 = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") float FrameMsMax = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") float GameThreadMsP50 = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") float GameThreadMsP95 = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") float FrameBudgetMs = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") bool bPaused = false;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") bool bPreviewActive = false;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int64 PreviewGeneration = 0;

	/** Process-wide leak counters: training states alive / with GPU buffers, preview buffer sets (render-thread
	    releases: read a few frames after a cancel). */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 LiveTrainingStates = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 AllocatedTrainingStates = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") int32 PreviewBufferSets = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") FString Message;
	/** "OK: ..." (Done, the TrainSplat result line) or "ERROR: ...". */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") FString Result;
	/** Object path of the last asset SaveAsset wrote. */
	UPROPERTY(BlueprintReadOnly, Category = "Harmony|Training") FString SavedAsset;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHarmonyTrainingFinished, bool, bSuccess, const FString&, Result);

/**
 * In-editor, non-blocking splat training (HarmonyTraining Phase 4, piece P3; CONTRACT_P4.md section 3). Drives one
 * FHarmonyTrainingSession per editor frame (FTickableEditorObject), optionally draws the training splats live with
 * Harmony's own renderer (a transient preview actor + zero-copy external buffers), and saves the result with the P1
 * asset writer. Python: s = unreal.get_editor_subsystem(unreal.HarmonyTrainingEditorSubsystem); s.start_training(req)
 * returns at once ("" or the error); poll s.get_status() or bind s.on_finished. Needs a ticking editor: launch probe
 * editors with -ExecCmds "py <script>" (a -run=pythonscript commandlet never ticks; use TrainSplat there).
 * One session at a time; starting a new one discards the previous (finished) one.
 */
UCLASS()
class HARMONYTRAININGEDITOR_API UHarmonyTrainingEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Starts a session. Returns "" on success or the error (environment, options, a session already running). */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	FString StartTraining(const FHarmonyTrainingRequest& Request);

	/**
	 * C++ only (Phase 4 P4, UHarmonyTrainingMoviePipelineOutput): StartTraining on an already built, in-memory dataset.
	 * Request.ColmapSparseDir / ImagesDir are not read (Init "colmap" reads Dataset->SparseDir); every other field is
	 * used as in StartTraining. The dataset must stay unchanged while the session holds it. Returns "" or the error.
	 */
	FString StartTrainingWithDataset(const FHarmonyTrainingRequest& Request, const TSharedPtr<const FHarmonyTrainingDataset>& Dataset);

	/**
	 * Phase 6: StartTraining on a cached capture dataset (a folder under the project's DatasetCacheRoot, or its name; see
	 * HarmonyTrainingDatasetCache.h). Request.ColmapSparseDir / ImagesDir are taken from the cache; Request.Init "" or
	 * "colmap" = the cache's points3D (the same init the capture's session used). Returns "" or the error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	FString StartTrainingFromCache(const FString& CacheDir, const FHarmonyTrainingRequest& Request);

	/** Phase 6: cached capture datasets, newest first (absolute folders). */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	TArray<FString> ListDatasetCaches() const;

	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	void PauseTraining();

	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	void ResumeTraining();

	/** Cancels without waiting for the GPU (the release is enqueued). */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	void CancelTraining();

	/** Ends after the steps already enqueued and finalizes (files, eval, asset) as if Steps were reached. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	void StopAndFinalize();

	/** True while a session is loading, training, paused or finalizing. */
	UFUNCTION(BlueprintPure, Category = "Harmony|Training")
	bool IsTraining() const;

	/** True while a finished session's Request.AssetPath is converted, compressed and committed (GetStatus().State "Saving";
	    IsTraining() is true too). CancelTraining no longer stops anything then: the asset will be written. */
	UFUNCTION(BlueprintPure, Category = "Harmony|Training")
	bool IsSavingAsset() const { return SaveJob.IsValid(); }

	UFUNCTION(BlueprintPure, Category = "Harmony|Training")
	FHarmonyTrainingStatus GetStatus() const;

	/** Clears the frame-time statistics of GetStatus (e.g. after a warm-up). */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	void ResetFrameStats();

	/** Enables / disables the live preview of the running session (it starts once the state is uploaded). */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	void SetLivePreview(bool bEnabled);

	/** Writes the finished session's splats with HarmonyTrainingEditor::WriteHarmonyAsset (load-then-overwrite, non-finite
	    rows dropped, package saved). Returns "" or the error. Game thread, blocking. (Request.AssetPath is saved without
	    blocking instead: converted and compressed on a worker while the status reads "Saving", then committed.) */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	FString SaveAsset(const FString& LongPackageName, bool bCompress = true);

	/**
	 * Level-viewport pose of the ValIndex-th val camera of the running session (UE world: location cm, rotation, and the
	 * camera's horizontal FOV in degrees), from its COLMAP pose: C = -R^T t, UE = 100 * (x, -z, -y); forward = row 2 of
	 * R and up = -(row 1) mapped the same way. False (Python: None) before the dataset is loaded.
	 */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Training")
	bool GetValCameraView(int32 ValIndex, FVector& Location, FRotator& Rotation, float& HorizontalFovDegrees) const;

	UPROPERTY(BlueprintAssignable, Category = "Harmony|Training")
	FOnHarmonyTrainingFinished OnFinished;

	// ---- C++ (ticker) ----
	void TickTraining(float DeltaSeconds);
	bool NeedsTick() const;

private:
	FString StartTrainingInternal(const FHarmonyTrainingRequest& Request, const TSharedPtr<const FHarmonyTrainingDataset>& Dataset);
	void ShutdownSession(bool bFlush);
	void ActivatePreview();
	void DeactivatePreview();
	void EnsurePreviewActor();
	void OnSessionFinished();
	/** Commits the async end-of-training save (waits for its worker if needed) and broadcasts OnFinished. */
	void FinishAsyncSave();
	bool ShouldDisableCPUThrottling() const;
	void HandleEditorPreExit();
	void HandlePreBeginPIE(const bool bIsSimulating);
	void HandleEndPIE(const bool bIsSimulating);
	void HandleMapOpened(const FString& Filename, bool bAsTemplate);
	void HandleWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);

	TSharedPtr<FHarmonyTrainingSession> Session;
	TSharedPtr<FHarmonyTrainingEditorTicker> Ticker;
	FHarmonyTrainingRequest ActiveRequest;

	/** Transient Expanded placeholder (StateCapacity empty rows) shown by the preview actor; Harmony draws the external
	    training buffers in its place (HarmonyExternalSplatSource.h). */
	UPROPERTY(Transient)
	TObjectPtr<UHarmonyDataAsset> PreviewPlaceholder;

	TWeakObjectPtr<AActor> PreviewActor;
	TSharedPtr<FHarmonyExternalSplatSource, ESPMode::ThreadSafe> PreviewSource;
	bool bPreviewWanted = false;
	bool bPausedForPIE = false;
	bool bFinishedHandled = true;
	bool bThrottleDelegateRegistered = false;
	FString SavedAssetPath;
	/** The end-of-training asset save (Request.AssetPath) while its build runs on a worker; null otherwise. */
	TSharedPtr<HarmonyTrainingEditor::FHarmonyAssetWriteJob> SaveJob;
	/** FinishAsyncSave is inside Commit (guards against re-entry from the Slate pumping of SavePackages). */
	bool bCommittingSave = false;
	/** The training result line, broadcast once SaveJob is committed. */
	FString PendingTrainingResult;

	TArray<float> FrameMsSamples;
	TArray<float> GameThreadMsSamples;

	FDelegateHandle PreExitHandle;
	FDelegateHandle PreBeginPIEHandle;
	FDelegateHandle EndPIEHandle;
	FDelegateHandle MapOpenedHandle;
	FDelegateHandle WorldCleanupHandle;
};
