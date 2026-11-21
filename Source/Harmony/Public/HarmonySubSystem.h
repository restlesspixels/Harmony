#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "Subsystems/EngineSubsystem.h"

#include "HarmonySubSystem.generated.h"

class FHarmonyViewExtension;
class FHarmonyExternalSplatSource;

UCLASS()
class HARMONY_API UHarmonySubsystem : public UEngineSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /**
     * The last Harmony component of World was unregistered: the renderer's splat buffers and proxy targets are freed on
     * the next tick, if the world still has no registered component then. An editor edit unregisters and registers the
     * component in one call; freeing at once left a frame without splats and forced a full re-upload.
     */
    void RequestReleaseSplatRenderResourcesIfUnused(const UWorld* World);

    /** Frees the renderer's splat buffers and proxy targets now (the next frame uploads again). */
    void RequestReleaseSplatRenderResources();

    /**
     * Zero-copy external splat source (HarmonyExternalSplatSource.h; HarmonyTraining live preview). One source at a time:
     * registering replaces the previous one. Game thread. Unregister only clears it if Source is the registered one.
     */
    void RegisterExternalSplatSource(const TSharedPtr<FHarmonyExternalSplatSource, ESPMode::ThreadSafe>& Source);
    void UnregisterExternalSplatSource(const TSharedPtr<FHarmonyExternalSplatSource, ESPMode::ThreadSafe>& Source);

protected:
    TSharedPtr<FHarmonyViewExtension, ESPMode::ThreadSafe> HarmonyViewExtension;
    TArray<TWeakObjectPtr<const UWorld>> PendingReleaseWorlds;
    FTSTicker::FDelegateHandle PendingReleaseTicker;
};
