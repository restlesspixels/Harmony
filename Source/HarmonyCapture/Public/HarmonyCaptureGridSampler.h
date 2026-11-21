// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HarmonyCaptureGridSampler.generated.h"

class AHarmonyCaptureRig;
class UHarmonyCaptureBoundsComponent;

/**
 * Grid camera sampler of AHarmonyCaptureRig (port of RotorSplatRenderTools' UGridCameraSampler): one marker every
 * GridSpacing cm over each include box, in the box's local frame, from -extent upwards, skipping exclude boxes.
 */
UCLASS()
class HARMONYCAPTURE_API UHarmonyCaptureGridSampler : public UObject
{
    GENERATED_BODY()

public:
    /** Replaces the rig's markers; returns the number spawned. */
    int32 SpawnGridComponents(AHarmonyCaptureRig* InCaptureRig, float GridSpacing, TArray<UHarmonyCaptureBoundsComponent*> AddVolumes, TArray<UHarmonyCaptureBoundsComponent*> RemoveVolumes);

private:
    void ClearPreviousLocations();
    int32 SpawnComponentsInVolume(UHarmonyCaptureBoundsComponent* Volume, float GridSpacing, const TArray<UHarmonyCaptureBoundsComponent*>& RemoveVolumes);
    bool IsPointInsideRemoveVolumes(const FVector& Point, const TArray<UHarmonyCaptureBoundsComponent*>& RemoveVolumes);
    void SpawnComponentAtPoint(const FVector& Point);
    bool ValidatePoint(const FVector& Point, TArray<UHarmonyCaptureBoundsComponent*> AddVolumes, TArray<UHarmonyCaptureBoundsComponent*> RemoveVolumes, float GridSpacing);

    UPROPERTY()
    TObjectPtr<AHarmonyCaptureRig> CaptureRig;
};
