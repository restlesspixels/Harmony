// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "HarmonyCaptureEditorHooks.h"
#include "HarmonyCaptureRig.h"
#include "TickableEditorObject.h"

#include "HarmonyCaptureJobSubsystem.generated.h"

class SNotificationItem;
class ULevelSequence;
class UMoviePipelineExecutorBase;
class UMoviePipelineQueue;

/**
 * One-click capture (the capture rig's Capture buttons): no Level Sequence, Movie Render Queue preset or post process
 * volume to set up by hand. From the rig in the open level it builds, in memory:
 *   - a Level Sequence that steps the rig's ShotIndex through every shot (one frame each) with a camera cut to the rig;
 *   - a Movie Render Queue job (deferred, square Resolution, TemporalSamples, Harmony Training output, the exposure lock
 *     and lens-effect switches of the rig's Capture settings);
 * renders it in Play In Editor and then:
 *   - Capture Only: keeps the dataset cache (Saved/HarmonyTraining/Datasets/<map>_*) for TrainFromCache;
 *   - Capture and Train: trains the splat in the editor (live preview) into the rig's OutputAsset (its capture is cached
 *     too, whatever the project's Cache Capture Datasets says: Retrain and Reuse Base need the asset's own last capture);
 *   - Bake Variants: captures and trains the base as above, then, per option of the rig's Variant Source (only the sets
 *     in VariantSetsToBake, all when empty), applies the option, captures it, restores the base and bakes the option into
 *     the asset as a sparse appearance delta (BuildVariantMasks -> TrainVariant -> BakeVariantOption).
 * Python: unreal.get_editor_subsystem(unreal.HarmonyCaptureJobSubsystem).capture_and_train(rig) etc.; is_busy(),
 * get_status(), get_last_result().
 */
UCLASS()
class HARMONYTRAININGEDITOR_API UHarmonyCaptureJobSubsystem : public UEditorSubsystem, public FTickableEditorObject
{
	GENERATED_BODY()

public:
	virtual void Deinitialize() override;

	/** Each returns "" when the job started, else why it could not (also shown as a notification). */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Capture")
	FString CaptureOnly(AHarmonyCaptureRig* Rig);

	UFUNCTION(BlueprintCallable, Category = "Harmony|Capture")
	FString CaptureAndTrain(AHarmonyCaptureRig* Rig);

	UFUNCTION(BlueprintCallable, Category = "Harmony|Capture")
	FString BakeVariants(AHarmonyCaptureRig* Rig);

	/** Trains the rig's output asset again from its newest base capture (FindLastBaseCapture): no render. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Capture")
	FString RetrainFromLastCapture(AHarmonyCaptureRig* Rig);

	/** The newest dataset cache of this map's base capture for OutputAsset ("" when there is none): its manifest's job is
	    <map>_Base (exactly: not a variant set named Base, not another map sharing the prefix) and its out_asset is
	    OutputAsset (a long package name; ".Object" is ignored). Caches written before out_asset was recorded match any
	    asset, and OutputAsset "" matches any cache of the map. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Capture")
	static FString FindLastBaseCapture(const FString& MapName, const FString& OutputAsset);

	/** The newest dataset cache of this map's capture of one variant option ("" when there is none): its manifest's job is
	    <map>_<set>_<option> (the name Bake Variants gives the option's capture) and its out_asset matches as in
	    FindLastBaseCapture. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Capture")
	static FString FindLastOptionCapture(const FString& MapName, const FString& OutputAsset, FName VariantSet, FName Option);

	/** Stops the render or the training (a variant bake in progress finishes its current option first). Once the trained
	    asset is being saved the save cannot be stopped: the job then ends after it, reporting the asset as written. */
	UFUNCTION(BlueprintCallable, Category = "Harmony|Capture")
	void Cancel();

	UFUNCTION(BlueprintPure, Category = "Harmony|Capture")
	bool IsBusy() const { return Phase != EPhase::Idle; }

	/** What the job is doing now. */
	UFUNCTION(BlueprintPure, Category = "Harmony|Capture")
	FString GetStatus() const { return Status; }

	/** How the last job ended: "OK: ..." or "ERROR: ..." ("" before the first one ends). */
	UFUNCTION(BlueprintPure, Category = "Harmony|Capture")
	FString GetLastResult() const { return LastResult; }

	/** The rig buttons' entry point (FHarmonyCaptureEditorHooks, bound by the module). */
	void HandleRequest(AHarmonyCaptureRig* Rig, EHarmonyCaptureRequest Request);

	//~ FTickableEditorObject
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }

private:
	enum class EPhase : uint8
	{
		Idle,
		Rendering,      // the Movie Render Queue executor runs
		AfterRender,    // waiting for its Play In Editor session to end
		Training,       // the base trains in UHarmonyTrainingEditorSubsystem
		NextRender,     // waiting for the training session to let go of the GPU
		Baking          // the next tick bakes the variant just captured (blocking, with a progress dialog)
	};

	struct FStep
	{
		FName Set;          // None: the base capture
		FName Option;
		FName BaseOption;   // what the set shows when the job starts (restored after the option's capture)
		FString CacheName;
		FString CacheDir;
		bool bApplied = false;
	};

	FString Start(AHarmonyCaptureRig* InRig, EHarmonyCaptureRequest InRequest);
	bool BuildSequence(AHarmonyCaptureRig& InRig, int32 NumShots, FString& OutError);
	bool StartRender(FString& OutError);
	void OnExecutorFinished(UMoviePipelineExecutorBase* InExecutor, bool bSuccess);
	void AfterRender();
	bool StartBaseTraining(FString& OutError, const FString& FromCache = FString());
	UFUNCTION()
	void OnTrainingFinished(bool bSuccess, const FString& Result);
	void BakeCurrentStep();
	void AdvanceToNextStep();
	void RestoreVariant(FStep& Step);
	/** True: lock the capture's exposure at OutEV100 (else the scene's own exposure is used as it is). bOutKnown: OutEV100 is
	    the exposure the views are rendered at, also when the scene locks it itself (16 Nov 2026: then it goes to the lighting
	    probe only, so the Harmony actor shows the level at the splats' exposure). */
	bool ResolveExposureLock(UWorld* World, float& OutEV100, bool& bOutKnown, FString& OutDescription) const;
	void Finish(bool bSuccess, const FString& Message);
	void SetStatus(const FString& Text);
	void UnbindTraining();

	UPROPERTY(Transient)
	TObjectPtr<ULevelSequence> Sequence;

	UPROPERTY(Transient)
	TObjectPtr<UMoviePipelineQueue> Queue;

	UPROPERTY(Transient)
	TObjectPtr<UMoviePipelineExecutorBase> Executor;

	TWeakObjectPtr<AHarmonyCaptureRig> Rig;
	TWeakObjectPtr<UWorld> World;
	EHarmonyCaptureRequest Request = EHarmonyCaptureRequest::CaptureOnly;
	FHarmonyCaptureJobSettings Settings;
	FString MapName;
	FString OutputAsset;
	int32 NumShots = 0;
	bool bLockExposure = false;
	float LockEV100 = 0.0f;           // with bLockExposure: the EV100 the capture is locked at (ResolveExposureLock)
	bool bExposureKnown = false;      // LockEV100 is the views' exposure (locked by the job or by the scene)
	int64 RenderCaptureCount = 0;     // GetLastCaptureCount when the current render started (AfterRender: is the last result ours?)
	TArray<FStep> Steps;
	int32 StepIndex = 0;
	EPhase Phase = EPhase::Idle;
	bool bRenderSucceeded = false;
	bool bCancelRequested = false;
	bool bTrainingBound = false;
	TArray<FString> Baked;
	FString Status;
	FString LastResult;
	double PhaseStartSeconds = 0.0;
	TSharedPtr<SNotificationItem> Notification;
};
