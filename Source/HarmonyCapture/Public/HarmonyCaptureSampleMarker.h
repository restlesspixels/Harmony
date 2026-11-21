// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "HarmonyCaptureSampleMarker.generated.h"

/**
 * One camera location of an AHarmonyCaptureRig (port of RotorSplatRenderTools' UCameraSampleMarker). Shown in the editor
 * as a small cube (a visualization component: hidden in game, no collision, no shadow); its world location is the camera
 * position and GetCameraRotation decodes the local shot into a pitch / yaw.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HARMONYCAPTURE_API UHarmonyCaptureSampleMarker : public UStaticMeshComponent
{
    GENERATED_BODY()

public:
    UHarmonyCaptureSampleMarker();

    /** Number of evenly spaced yaw samples across [0, 360). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Rotation", meta=(ClampMin="1", UIMin="1"))
    int32 NumberOfYawSteps = 8;

    /** Number of inclusive pitch samples from +90 degrees to -90 degrees. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Rotation", meta=(ClampMin="2", UIMin="2"))
    int32 NumberOfPitchSteps = 5;

    /** Rows sampling: this position's own shots (world rotations), in order. Empty: the yaw / pitch steps above. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera Rotation")
    TArray<FRotator> ShotRotations;

    /** Total number of camera angles at this location: ShotRotations, else 2 + (pitch steps - 2) * yaw steps. */
    UFUNCTION(BlueprintCallable, Category="Camera Settings")
    int32 GetTotalAngles() const;

    /** The camera rotation for a local shot index. */
    UFUNCTION(BlueprintCallable, Category="Camera Rotation")
    FRotator GetCameraRotation(int32 LocalShotIndex) const;

    /** Decodes a local shot, with a single yaw sample at each pitch pole. */
    UFUNCTION(BlueprintPure, Category="Camera Rotation")
    void GetShotIndices(int32 LocalShotIndex, int32& OutYawIndex, int32& OutPitchIndex) const;

protected:
    virtual void OnComponentCreated() override;

    /** Gives the marker a green dynamic material instance (editor visualisation only). */
    void SetupMeshAndMaterial();
};
