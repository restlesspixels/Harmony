// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "HarmonyCaptureBoundsComponent.generated.h"

class AHarmonyCaptureRig;

/**
 * An include or exclude box of an AHarmonyCaptureRig (port of RotorSplatRenderTools' UCaptureBoundsComponent). The rig
 * attaches it to its root, colours it (include green, exclude red) and places it at BoundsLocation relative to the rig.
 */
UCLASS()
class HARMONYCAPTURE_API UHarmonyCaptureBoundsComponent : public UBoxComponent
{
    GENERATED_BODY()

public:
    /** Location relative to the owning capture rig, allowing each bounds entry to be positioned independently. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shape", meta=(DisplayName="Location"))
    FVector BoundsLocation = FVector::ZeroVector;

    void Setup(AHarmonyCaptureRig* InCaptureRig);

protected:
    UPROPERTY()
    TObjectPtr<AHarmonyCaptureRig> CaptureRig;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
