// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HarmonyCaptureRadialSampler.generated.h"

class AHarmonyCaptureRig;
class UHarmonyCaptureBoundsComponent;

/**
 * Radial camera sampler of AHarmonyCaptureRig (port of RotorSplatRenderTools' URadialCameraSampler): markers on the
 * ellipse inscribed in each include box (radii = box X / Y extent), AngularDivisions per ring, NumberOfHeights rings
 * from the bottom to the top of the box (one ring in the middle when NumberOfHeights is 1).
 */
UCLASS()
class HARMONYCAPTURE_API UHarmonyCaptureRadialSampler : public UObject
{
    GENERATED_BODY()

public:
    /** Replaces the rig's markers; returns the number spawned. */
    int32 SpawnRadialComponents(AHarmonyCaptureRig* InCaptureRig, int32 NumberOfHeights, int32 AngularDivisions, TArray<UHarmonyCaptureBoundsComponent*> AddVolumes, TArray<UHarmonyCaptureBoundsComponent*> RemoveVolumes);

private:
    bool ValidatePoint(const FVector& Point, TArray<UHarmonyCaptureBoundsComponent*> AddVolumes, TArray<UHarmonyCaptureBoundsComponent*> RemoveVolumes, float Spacing);
    void ClearPreviousLocations();
    void SpawnComponentAtPoint(const FVector& Point, const FVector& CenterPoint);

    UPROPERTY()
    TObjectPtr<AHarmonyCaptureRig> CaptureRig;
};
