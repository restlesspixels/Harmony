#pragma once

#include "CoreMinimal.h"

// Exported (HarmonyTraining Phase 4, CONTRACT_P4.md P1) so HarmonyTrainingEditor converts trained splats with exactly the
// frame, position and covariance code the PLY importer uses (HarmonyPlyImporter.cpp). Private/Import/HarmonySplatOps.h
// includes this header, so every existing "Import/HarmonySplatOps.h" include keeps working.
struct HARMONYEDITOR_API FHarmonySplatOps
{
    struct FMat3
    {
        float M[3][3] = {
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f}
        };
    };

    struct FFrame
    {
        FVector3f AxisX = FVector3f(1.0f, 0.0f, 0.0f);
        FVector3f AxisY = FVector3f(0.0f, 1.0f, 0.0f);
        FVector3f AxisZ = FVector3f(0.0f, 0.0f, 1.0f);
    };

    static FFrame IdentityFrame();
    static FFrame WebSplatToUnrealFrame();

    static FVector3f TransformDirection(const FVector3f& InDirection, const FFrame& Frame);
    static FVector3f TransformPosition(const FVector3f& InPosition, const FFrame& Frame, float UnitScale);

    static FMat3 Multiply(const FMat3& A, const FMat3& B);
    static FMat3 Transpose(const FMat3& In);

    static FVector3f RotateVectorByQuaternionWXYZ(const FVector3f& InVector, const FVector4f& InQuaternionWXYZ);

    static FMat3 FrameToMatrix(const FFrame& Frame);
    static FMat3 CovarianceFromPacked(const FVector4f& Sigma0, const FVector4f& Sigma1);
    static void  CovarianceToPacked(const FMat3& Cov, FVector4f& OutSigma0, FVector4f& OutSigma1);

    static void BuildCovarianceFromScaleAndRotation(
        const FVector3f& Scale,
        bool bScaleIsLog,
        const FVector4f& RotationWXYZ,
        const FFrame& Frame,
        float UnitScale,
        bool bTransposeFrameTransform,
        FVector4f& OutSigma0,
        FVector4f& OutSigma1);

    static void BuildCovarianceFromRawPly(
        const FVector3f& LogScale,
        const FVector4f& RotationWXYZ,
        const FFrame& Frame,
        float UnitScale,
        bool bTransposeFrameTransform,
        FVector4f& OutSigma0,
        FVector4f& OutSigma1);

    static void BuildCovarianceFromPackedSplat(
        const FVector3f& Scale,
        const FVector4f& RotationWXYZ,
        const FFrame& Frame,
        float UnitScale,
        bool bTransposeFrameTransform,
        FVector4f& OutSigma0,
        FVector4f& OutSigma1);
};
