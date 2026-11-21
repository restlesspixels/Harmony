// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HarmonyTrainingDataset.h"
#include "HarmonyTrainingTypes.h"
#include "RHIResources.h"            // FShaderResourceViewRHIRef
#include "Templates/Function.h"
#include "Templates/SharedPointer.h"

/**
 * HarmonyTraining Phase 4, piece P3 (H:/SplatTools/spike/CONTRACT_P4.md section 3; design notes/phase4_editor.md 2-3):
 * a training run driven one game-thread tick at a time, so the editor keeps rendering while it trains.
 *
 * Same trainer core as the headless UHarmonyTrainingLibrary::TrainSplat (HarmonyTrainingTrainerCore.h): same options,
 * same per-step passes, same image order, MCMC schedule, PLY / state dir / summary files and "OK: ..." result line.
 * Only the driver differs:
 * - Tick never blocks: no FlushRenderingCommands, no FRenderCommandFence::Wait, no GPU-idle waits, no manual frame pump.
 *   Setup (init splats + image decode) runs on a UE::Tasks worker; the upload is polled with a render command fence;
 *   chunks are gated on the render thread's consumed-chunk counter (at most RingSize = 3 in flight); loss readbacks are
 *   consumed only when ready; the final state readback, the eval graphs and the file writes are polled / run on tasks.
 * - Steps per tick adapt to a frame budget (FrameBudgetMs; 0 = turbo, MaxStepsPerTick every tick).
 * - Pause / Resume / StopEarly / Cancel. Cancel(bFlush = false) only enqueues the release; bFlush = true (shutdown paths:
 *   editor exit, module unload) also flushes the render thread.
 * - Optional zero-copy live preview: after every preview interval a render-thread pass converts the training state into
 *   Harmony's Expanded per-splat layout (HTHarmonyExpand.usf) and hands the buffers to a render-thread sink (the editor
 *   module forwards them to Harmony's renderer). No CPU readback.
 *
 * Game thread only (except where marked). Not a UObject: hold it in a TSharedPtr; destroying it cancels without flushing.
 */

enum class EHarmonyTrainingState : uint8
{
	Idle,
	Loading,      // worker task: init splats (+ COLMAP dataset load / image decode)
	Uploading,    // InitGPUState render command (one render-thread stall of a few hundred ms at most)
	Training,     // chunks enqueued per tick
	Paused,       // nothing new enqueued; pending readbacks still land
	Draining,     // all steps enqueued; waiting for the last readbacks
	Finalizing,   // final state readback -> files -> out_asset hook -> eval -> summary
	Done,
	Failed,
	Cancelled
};

HARMONYTRAINING_API const TCHAR* LexToString(EHarmonyTrainingState State);

struct HARMONYTRAINING_API FHarmonyTrainingSessionParams
{
	/** Init splats: state dir, PLY, or "colmap" (points3D of ColmapSparseDir / Dataset->SparseDir). */
	FString Init = TEXT("colmap");
	/** COLMAP text model + images folder (downscale from its "_<n>" suffix), loaded on the worker like TrainSplat... */
	FString ColmapSparseDir;
	FString ImagesDir;
	/** ...or an already loaded dataset (takes precedence when set; must stay unchanged while the session uses it). */
	TSharedPtr<const FHarmonyTrainingDataset> Dataset;

	int32 Steps = 100000;
	/** The TrainSplat option keys (HarmonyTrainingLibrary.h). "eval_out_dir" is ignored (warning) by sessions. */
	FString OptionsJson;
	/** Optional: PLY + <base>_train.json / _loss.f32. Empty = no files (the final splats stay in GetFinalSplats()). */
	FString OutPly;

	/** Frame budget of one editor frame (training included), ms. 0 = turbo (MaxStepsPerTick every tick). */
	float FrameBudgetMs = 33.0f;
	int32 MinStepsPerTick = 1;
	int32 MaxStepsPerTick = 64;   // clamped to MaxStepsPerChunk (64)

	/** Minimum time between two live-preview conversions (0 = every tick in which the step advanced). */
	float PreviewIntervalSeconds = 0.1f;
};

/** Game-thread progress snapshot (updated by Tick). */
struct HARMONYTRAINING_API FHarmonyTrainingProgress
{
	EHarmonyTrainingState State = EHarmonyTrainingState::Idle;
	int32 Step = 0;               // consumed steps (results known)
	int32 Steps = 0;              // planned steps (StopEarly lowers it)
	int32 EnqueuedSteps = 0;
	uint32 NumSplats = 0;         // logical N after the last consumed step
	uint32 StateCapacity = 0;     // rows of the GPU state (cap_max for MCMC); 0 before the upload
	float Loss = 0.0f;            // last consumed step
	float Psnr = 0.0f;            // train PSNR of the last consumed step (from its mse)
	float MsPerStep = 0.0f;       // wall time per consumed step (EMA)
	int32 StepsPerTick = 0;       // current adaptive K
	float FrameMs = 0.0f;         // controller's smoothed frame time
	float GpuFrameMs = 0.0f;      // RHIGetGPUFrameCycles of the last frame
	int32 OverflowSteps = 0;
	int32 EvalDone = 0;
	int32 EvalTotal = 0;
	double ValPsnrMean = 0.0;     // valid in Done when eval ran
	double ValPsnrMin = 0.0;
	uint64 PreviewGeneration = 0; // preview conversions enqueued
	FString Message;              // last state change / error
	FString Result;               // "OK: ..." (Done) or "ERROR: ..." (Failed)
};

/**
 * Harmony Expanded-layout buffers filled from the training state (HTHarmonyExpand.usf). Render thread only. NumRows =
 * the state capacity: rows >= the logical N have alpha 0. Layout: PosRadius float4 N (UE cm, w = 1), Color float4 N
 * (0, 0, 0, opacity), Covariance float4 2N (cm^2), SHCoeffs float4 16N (raw, (r,g,b,0)). Every buffer is left in
 * ERHIAccess::SRVMask after the conversion graph, so the SRVs may be bound with raw RHI calls until the next conversion.
 * Generation increases by one per conversion (0 = never converted).
 */
struct HARMONYTRAINING_API FHarmonyTrainingPreviewBuffers
{
	TRefCountPtr<FRDGPooledBuffer> PosRadius;
	TRefCountPtr<FRDGPooledBuffer> Color;
	TRefCountPtr<FRDGPooledBuffer> Covariance;
	TRefCountPtr<FRDGPooledBuffer> SHCoeffs;
	FShaderResourceViewRHIRef PosRadiusSRV;
	FShaderResourceViewRHIRef ColorSRV;
	FShaderResourceViewRHIRef CovarianceSRV;
	FShaderResourceViewRHIRef SHCoeffsSRV;
	uint32 NumRows = 0;
	uint32 NumSplats = 0;      // logical N converted
	int32 Step = 0;            // steps enqueued before the conversion
	uint64 Generation = 0;

	bool IsValid() const
	{
		return NumRows > 0 && PosRadiusSRV.IsValid() && ColorSRV.IsValid() && CovarianceSRV.IsValid() && SHCoeffsSRV.IsValid();
	}
};

/** Render-thread receiver of every preview conversion (and of an empty FHarmonyTrainingPreviewBuffers when released). */
using FHarmonyTrainingPreviewSink = TFunction<void(const FHarmonyTrainingPreviewBuffers&)>;

namespace HarmonyTraining
{
namespace Trainer
{
	class FTrainerCore;
}
	/** P3 leak check: live training GPU states (FTrainRT objects / with allocated pooled buffers) and allocated preview
	    buffer sets, process wide. The release of each is a render command: read after the render thread caught up. */
	HARMONYTRAINING_API void GetTrainingResourceCounts(int32& OutLiveStates, int32& OutAllocatedStates, int32& OutPreviewBufferSets);
}

class HARMONYTRAINING_API FHarmonyTrainingSession : public TSharedFromThis<FHarmonyTrainingSession>
{
public:
	/** Validates the environment and the options on the game thread (TrainSplat's checks, same messages) and launches the
	    loading task. nullptr + OutError (no "ERROR: " prefix) on failure. */
	static TSharedPtr<FHarmonyTrainingSession> Create(const FHarmonyTrainingSessionParams& Params, FString& OutError);

	~FHarmonyTrainingSession();
	FHarmonyTrainingSession(const FHarmonyTrainingSession&) = delete;
	FHarmonyTrainingSession& operator=(const FHarmonyTrainingSession&) = delete;

	/** Advances the state machine by one step. Never blocks. DeltaSeconds = the engine frame time (adaptive K). */
	void Tick(float DeltaSeconds);

	/** Stop enqueuing chunks (pending results still land). Allowed in Loading / Uploading (applies once trained). */
	void Pause();
	void Resume();
	bool IsPaused() const;

	/** End after the steps already enqueued, then finalize as if Steps were reached (an early checkpoint). */
	void StopEarly();

	/** Cancelled: enqueues the GPU release (and the preview release); bFlush also waits for the render thread and the
	    worker tasks (shutdown paths only). Idempotent; a finished session only releases its preview buffers. */
	void Cancel(bool bFlush);

	bool IsFinished() const;   // Done, Failed or Cancelled
	EHarmonyTrainingState GetState() const { return Progress.State; }
	const FHarmonyTrainingProgress& GetProgress() const { return Progress; }
	const FHarmonyTrainingSessionParams& GetParams() const { return Params; }

	/** (loss, l1, ssim, mse) of every consumed step; valid until the next Tick. */
	TConstArrayView<FVector4f> GetLossHistory() const;

	/** The final splats (device layout, as written to the PLY), from Finalizing on until ReleaseFinalSplats. */
	TSharedPtr<const FHarmonyTrainingSplatsCPU> GetFinalSplats() const { return FinalSplats; }
	/** The run used option antialiased (Mip-Splatting's 2D filter): its asset must be drawn with the same compensation. */
	bool IsAntialiased() const;
	void ReleaseFinalSplats() { FinalSplats.Reset(); }

	/** Camera of the ValIndex-th val image (or of the TrainIndex-th train image) once the dataset is loaded. */
	bool GetValCamera(int32 ValIndex, FHarmonyTrainingCamera& OutCamera) const;
	bool GetTrainCamera(int32 TrainIndex, FHarmonyTrainingCamera& OutCamera) const;
	int32 GetNumValImages() const;

	// ---- Live preview ----
	/** Sets the render-thread sink (copied into each preview render command). */
	void SetPreviewSink(FHarmonyTrainingPreviewSink InSink);
	/** Starts / stops the preview conversions. Disabling releases the preview buffers and calls the sink with empty
	    buffers on the render thread. A finished session keeps its last conversion until disabled or destroyed. */
	void SetPreviewEnabled(bool bEnabled);
	bool IsPreviewEnabled() const { return bPreviewEnabled; }
	/** Requests a conversion on the next tick even when the step did not advance (e.g. after a view change). */
	void RequestPreviewRefresh() { bPreviewDirty = true; }

private:
	struct FLoadResult;
	struct FStateSnapshot;
	struct FEvalPending;
	struct FPreviewState;
	enum class EFinalizeStage : uint8 { ReadbackRequest, ReadbackPoll, WriteFiles, Eval, Summary };

	FHarmonyTrainingSession();
	void SetState(EHarmonyTrainingState NewState, const FString& Message = FString());
	void Fail(const FString& Message);
	void ReleaseGPU(bool bFlush);
	void TickLoading();
	void TickUploading();
	void TickTraining(float DeltaSeconds);
	void TickDraining();
	void TickFinalizing();
	void TickPreview();
	void UpdateStepsPerTick(float DeltaSeconds);
	void UpdateProgress();
	void ReleasePreviewBuffers();

	FHarmonyTrainingSessionParams Params;
	FHarmonyTrainingProgress Progress;
	TUniquePtr<HarmonyTraining::Trainer::FTrainerCore> Core;
	double StartTime = 0.0;

	// Loading
	TSharedPtr<FLoadResult> Load;
	bool bLoadTaskRunning = false;

	// Training
	bool bPauseRequested = false;
	bool bStopRequested = false;
	int32 StepsPerTick = 4;
	int32 GoodFrames = 0;
	float FrameMsEma = 0.0f;
	int32 LastMeasuredStep = 0;
	double LastMeasureTime = 0.0;
	double MsPerStepEma = 0.0;

	// Finalizing
	EFinalizeStage FinalizeStage = EFinalizeStage::ReadbackRequest;
	TSharedPtr<FStateSnapshot> Snapshot;
	TSharedPtr<FHarmonyTrainingSplatsCPU> FinalSplatsWork;   // owned by the file task while it runs
	TSharedPtr<const FHarmonyTrainingSplatsCPU> FinalSplats;
	struct FFileTaskResult;
	TSharedPtr<FFileTaskResult> FileTask;
	FString FullStateDir;
	int32 NumNonFiniteRows = 0;
	TSharedPtr<FEvalPending> EvalPending;
	int32 EvalNext = 0;
	struct FEvalSummaryHolder;
	TUniquePtr<FEvalSummaryHolder> EvalSummary;

	// Preview
	bool bPreviewEnabled = false;
	bool bPreviewDirty = true;
	bool bPreviewShaderChecked = false;
	bool bPreviewShaderAvailable = false;
	double LastPreviewTime = 0.0;
	int32 LastPreviewStep = -1;
	FHarmonyTrainingPreviewSink PreviewSink;
	TSharedPtr<FPreviewState> PreviewState;
};
