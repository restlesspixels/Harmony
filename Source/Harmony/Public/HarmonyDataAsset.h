#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "HarmonyDataAsset.generated.h"

class UStaticMesh;
class UMaterialInterface;

class UTextureCube;

UENUM()
enum class EHarmonyAssetEncoding : uint8
{
    Expanded,
    Compressed
};

UENUM()
enum class EHarmonyPrimitiveRepresentation : uint8
{
    Gaussian3D UMETA(DisplayName="3D Gaussian"),
    Billboard2D UMETA(DisplayName="BB Splat (Textured Billboard)")
};

UENUM()
enum class EHarmonyCompressionPreset : uint8
{
    Balanced UMETA(DisplayName="Balanced"),
    Smallest UMETA(DisplayName="Smallest")
};

inline constexpr int32 GHarmonyMaxSHCoefficients = 16;
inline constexpr int32 GHarmonyFullCompressedSHWordsPerFeature = (GHarmonyMaxSHCoefficients * 3) / 4;
inline constexpr int32 GHarmonyCompressedPositionBucketSize = 256;
inline constexpr int32 GHarmonyCompressionFormatVersion = 2;
inline constexpr int32 GHarmonyBBSplatTextureSize = 16;
inline constexpr int32 GHarmonyBBSplatTextureBytesPerPrimitive =
    GHarmonyBBSplatTextureSize * GHarmonyBBSplatTextureSize * 4;

FORCEINLINE int32 GetHarmonyStoredSHCoefficientCount(const int32 SHDegree)
{
    switch (SHDegree)
    {
        case 0: return 1;
        case 1: return 4;
        case 2: return 9;
        case 3: return GHarmonyMaxSHCoefficients;
        default: return SHDegree > 3 ? GHarmonyMaxSHCoefficients : 0;
    }
}

FORCEINLINE int32 GetHarmonyCompressedSHWordCount(const int32 SHDegree)
{
    const int32 StoredCoefficientCount = GetHarmonyStoredSHCoefficientCount(SHDegree);
    return StoredCoefficientCount > 0 ? ((StoredCoefficientCount * 3 + 3) / 4) : 0;
}

FORCEINLINE int32 GetHarmonyStoredRestSHCoefficientCount(const int32 SHDegree)
{
    return FMath::Max(GetHarmonyStoredSHCoefficientCount(SHDegree) - 1, 0);
}

FORCEINLINE int32 GetHarmonyCompressedRestSHByteCount(const int32 SHDegree)
{
    return GetHarmonyStoredRestSHCoefficientCount(SHDegree) * 3;
}

FORCEINLINE int32 GetHarmonyCompressedPositionBucketCount(const int32 NumPoints)
{
    return NumPoints > 0
        ? ((NumPoints + GHarmonyCompressedPositionBucketSize - 1) / GHarmonyCompressedPositionBucketSize)
        : 0;
}

USTRUCT()
struct FHarmonyCompressedQuantizationChannel
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    int32 ZeroPoint = 0;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    float Scale = 1.0f;
};

USTRUCT()
struct FHarmonyCompressedQuantization
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    FHarmonyCompressedQuantizationChannel ColorDC;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    FHarmonyCompressedQuantizationChannel ColorRest;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    FHarmonyCompressedQuantizationChannel Opacity;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    FHarmonyCompressedQuantizationChannel ScalingFactor;
};

USTRUCT()
struct FHarmonyCompressionFormat
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    int32 Version = 0;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    EHarmonyCompressionPreset Preset = EHarmonyCompressionPreset::Balanced;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    bool bUsesMortonOrdering = false;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    bool bUsesScaleCodebook = false;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    bool bUsesColorDCCodebook = false;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    bool bUsesSHPalette = false;
};

USTRUCT()
struct FHarmonyRawData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    FString SourceFilename;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    int32 NumPoints = 0;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    int32 SHDegree = 3;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    FVector3f BoundsMin = FVector3f::ZeroVector;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    FVector3f BoundsMax = FVector3f::ZeroVector;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    float EstimatedSerializedSizeMB = 0.0f;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    EHarmonyAssetEncoding Encoding = EHarmonyAssetEncoding::Expanded;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    EHarmonyPrimitiveRepresentation Representation = EHarmonyPrimitiveRepresentation::Gaussian3D;

    // Trained with Mip-Splatting's 2D filter (HarmonyTraining option antialiased): drawn with the same opacity compensation,
    // sqrt(det(cov2d) / det(cov2d + 0.3)), whatever r.Harmony.Tuning.Preprocess.OpacityCompensation says. Set it by hand on
    // an imported PLY that was trained that way (gsplat rasterize_mode "antialiased", Mip-Splatting).
    UPROPERTY(EditAnywhere, Category = "Gaussian Splat|Debug")
    bool bAntialiased = false;

    // Path of the asset this was decimated from, empty if not decimated.
    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    FString SourceAssetPath;

    // 1.0 = not decimated; otherwise the target keep-ratio passed to the decimate action.
    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    float DecimationTargetRatio = 1.0f;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Packed")
    float UnitScale = 100.0f;

    // Bulk packed data (direct upload layout) is intentionally hidden from details/blueprint panels.
    UPROPERTY()
    TArray<FVector4f> PosRadius;

    UPROPERTY()
    TArray<FVector4f> Color;

    UPROPERTY()
    TArray<FVector4f> Covariance;

    UPROPERTY()
    TArray<FVector4f> SHCoeffs;

    // BB Splat's two oriented, scaled plane axes. These are deliberately stored instead of a
    // covariance matrix because a symmetric covariance cannot preserve the texture's UV frame.
    UPROPERTY()
    TArray<FVector4f> BillboardAxisX;

    UPROPERTY()
    TArray<FVector4f> BillboardAxisY;

    // Per primitive, row-major RGBA8, with a fixed 16x16 footprint. Alpha is reconstructed from
    // BBSplat's residual encoding by the importer so the runtime texture is directly sampleable.
    UPROPERTY()
    TArray<uint8> BillboardTextureRGBA;

    bool HasValidBillboardData() const
    {
        return Representation == EHarmonyPrimitiveRepresentation::Billboard2D &&
            Encoding == EHarmonyAssetEncoding::Expanded &&
            NumPoints > 0 &&
            PosRadius.Num() >= NumPoints &&
            BillboardAxisX.Num() >= NumPoints &&
            BillboardAxisY.Num() >= NumPoints &&
            BillboardTextureRGBA.Num() >= (NumPoints * GHarmonyBBSplatTextureBytesPerPrimitive) &&
            GetExpandedSHStorageStride() > 0 &&
            SHCoeffs.Num() >= (NumPoints * GetExpandedSHStorageStride());
    }

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    int32 CompressedGeometryCount = 0;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    int32 CompressedFeatureCount = 0;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    FHarmonyCompressedQuantization CompressedQuantization;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    FHarmonyCompressionFormat CompressionFormat;

    UPROPERTY()
    TArray<FVector3f> CompressedPosition;

    UPROPERTY()
    TArray<uint16> CompressedPositionQuantized;

    UPROPERTY()
    TArray<FVector3f> CompressedPositionBucketCenter;

    UPROPERTY()
    TArray<FVector3f> CompressedPositionBucketExtent;

    UPROPERTY()
    TArray<uint32> CompressedMetadata;

    UPROPERTY()
    TArray<uint16> CompressedMetadataCompact;

    UPROPERTY()
    TArray<uint8> CompressedMetadataCodebookLabels;

    UPROPERTY()
    TArray<uint16> CompressedMetadataCodebook;

    UPROPERTY()
    TArray<uint32> CompressedCovariance;

    UPROPERTY()
    TArray<uint32> CompressedSHData;

    UPROPERTY()
    TArray<uint8> CompressedSHDataBytes;

    UPROPERTY()
    TArray<uint8> CompressedColorDCCodebookLabels;

    UPROPERTY()
    TArray<uint32> CompressedColorDCCodebook;

    int32 GetExpandedSHStorageStride() const
    {
        const int32 CompactCoefficientCount = GetHarmonyStoredSHCoefficientCount(SHDegree);
        if (CompactCoefficientCount <= 0)
        {
            return 0;
        }

        if (NumPoints > 0 && SHCoeffs.Num() >= (NumPoints * GHarmonyMaxSHCoefficients))
        {
            return GHarmonyMaxSHCoefficients;
        }

        return CompactCoefficientCount;
    }

    int32 GetCompactExpandedSHStorageCount() const
    {
        const int32 CompactCoefficientCount = GetHarmonyStoredSHCoefficientCount(SHDegree);
        return (NumPoints > 0 && CompactCoefficientCount > 0)
            ? (NumPoints * CompactCoefficientCount)
            : 0;
    }

    int32 GetCompressedSHStorageStrideWords() const
    {
        const int32 CompactWordCount = GetHarmonyCompressedSHWordCount(SHDegree);
        if (CompactWordCount <= 0)
        {
            return 0;
        }

        if (CompressedFeatureCount <= 0)
        {
            return 0;
        }

        if (CompressedFeatureCount > 0 &&
            CompressedSHData.Num() >= (CompressedFeatureCount * GHarmonyFullCompressedSHWordsPerFeature))
        {
            return GHarmonyFullCompressedSHWordsPerFeature;
        }

        return CompressedSHData.Num() >= (CompressedFeatureCount * CompactWordCount)
            ? CompactWordCount
            : 0;
    }

    int32 GetCompressedSHStorageStrideBytes() const
    {
        if (!HasCompressedColorDCCodebook())
        {
            return 0;
        }

        const int32 RestByteCount = GetHarmonyCompressedRestSHByteCount(SHDegree);
        if (RestByteCount == 0)
        {
            return 0;
        }

        return (CompressedFeatureCount > 0 && CompressedSHDataBytes.Num() >= (CompressedFeatureCount * RestByteCount))
            ? RestByteCount
            : 0;
    }

    bool HasBucketedCompressedPositions() const
    {
        const int32 BucketCount = GetHarmonyCompressedPositionBucketCount(NumPoints);
        return BucketCount > 0 &&
            CompressedPositionQuantized.Num() >= (NumPoints * 3) &&
            CompressedPositionBucketCenter.Num() >= BucketCount &&
            CompressedPositionBucketExtent.Num() >= BucketCount;
    }

    bool HasLegacyCompressedPositions() const
    {
        return NumPoints > 0 && CompressedPosition.Num() >= NumPoints;
    }

    bool HasCompactCompressedMetadata() const
    {
        return NumPoints > 0 &&
            CompressedGeometryCount == NumPoints &&
            CompressedFeatureCount == NumPoints &&
            CompressedMetadataCompact.Num() >= NumPoints;
    }

    bool HasCompressedMetadataCodebook() const
    {
        return NumPoints > 0 &&
            CompressionFormat.bUsesScaleCodebook &&
            CompressedGeometryCount == NumPoints &&
            CompressedFeatureCount == NumPoints &&
            CompressedMetadataCodebookLabels.Num() >= NumPoints &&
            CompressedMetadataCodebook.Num() > 0 &&
            CompressedMetadataCodebook.Num() <= 256;
    }

    bool HasCompressedColorDCCodebook() const
    {
        return CompressedFeatureCount > 0 &&
            CompressionFormat.bUsesColorDCCodebook &&
            CompressedColorDCCodebookLabels.Num() >= CompressedFeatureCount &&
            CompressedColorDCCodebook.Num() > 0 &&
            CompressedColorDCCodebook.Num() <= 256;
    }

    int32 GetCompressedMetadataStrideWords() const
    {
        if (NumPoints <= 0)
        {
            return 0;
        }

        if (CompressedMetadata.Num() >= (NumPoints * 3))
        {
            return 3;
        }

        return ((CompressedGeometryCount == NumPoints && CompressedFeatureCount == NumPoints && CompressedMetadata.Num() >= NumPoints) ||
                HasCompactCompressedMetadata() ||
                HasCompressedMetadataCodebook())
            ? 1
            : 0;
    }

    void UpdateDebugStats()
    {
        uint64 Bytes = 0;
        if (Encoding == EHarmonyAssetEncoding::Compressed)
        {
            Bytes =
                static_cast<uint64>(CompressedPosition.Num()) * sizeof(FVector3f) +
                static_cast<uint64>(CompressedPositionQuantized.Num()) * sizeof(uint16) +
                static_cast<uint64>(CompressedPositionBucketCenter.Num()) * sizeof(FVector3f) +
                static_cast<uint64>(CompressedPositionBucketExtent.Num()) * sizeof(FVector3f) +
                static_cast<uint64>(CompressedMetadata.Num()) * sizeof(uint32) +
                static_cast<uint64>(CompressedMetadataCompact.Num()) * sizeof(uint16) +
                static_cast<uint64>(CompressedMetadataCodebookLabels.Num()) * sizeof(uint8) +
                static_cast<uint64>(CompressedMetadataCodebook.Num()) * sizeof(uint16) +
                static_cast<uint64>(CompressedCovariance.Num()) * sizeof(uint32) +
                static_cast<uint64>(CompressedSHData.Num()) * sizeof(uint32) +
                static_cast<uint64>(CompressedSHDataBytes.Num()) * sizeof(uint8) +
                static_cast<uint64>(CompressedColorDCCodebookLabels.Num()) * sizeof(uint8) +
                static_cast<uint64>(CompressedColorDCCodebook.Num()) * sizeof(uint32);
        }
        else
        {
            Bytes =
                static_cast<uint64>(PosRadius.Num()) * sizeof(FVector4f) +
                static_cast<uint64>(Color.Num()) * sizeof(FVector4f) +
                static_cast<uint64>(Covariance.Num()) * sizeof(FVector4f) +
                static_cast<uint64>(SHCoeffs.Num()) * sizeof(FVector4f) +
                static_cast<uint64>(BillboardAxisX.Num()) * sizeof(FVector4f) +
                static_cast<uint64>(BillboardAxisY.Num()) * sizeof(FVector4f) +
                static_cast<uint64>(BillboardTextureRGBA.Num()) * sizeof(uint8);
        }

        EstimatedSerializedSizeMB = static_cast<float>(Bytes) / (1024.0f * 1024.0f);
    }

    void Reset()
    {
        SourceFilename.Empty();
        NumPoints = 0;
        SHDegree = 3;
        BoundsMin = FVector3f::ZeroVector;
        BoundsMax = FVector3f::ZeroVector;
        EstimatedSerializedSizeMB = 0.0f;
        Encoding = EHarmonyAssetEncoding::Expanded;
        Representation = EHarmonyPrimitiveRepresentation::Gaussian3D;
        SourceAssetPath.Empty();
        DecimationTargetRatio = 1.0f;
        UnitScale = 100.0f;
        PosRadius.Reset();
        Color.Reset();
        Covariance.Reset();
        SHCoeffs.Reset();
        BillboardAxisX.Reset();
        BillboardAxisY.Reset();
        BillboardTextureRGBA.Reset();
        CompressedGeometryCount = 0;
        CompressedFeatureCount = 0;
        CompressedQuantization = FHarmonyCompressedQuantization{};
        CompressedPosition.Reset();
        CompressedPositionQuantized.Reset();
        CompressedPositionBucketCenter.Reset();
        CompressedPositionBucketExtent.Reset();
        CompressedMetadata.Reset();
        CompressedMetadataCompact.Reset();
        CompressedMetadataCodebookLabels.Reset();
        CompressedMetadataCodebook.Reset();
        CompressedCovariance.Reset();
        CompressedSHData.Reset();
        CompressedSHDataBytes.Reset();
        CompressedColorDCCodebookLabels.Reset();
        CompressedColorDCCodebook.Reset();
        CompressionFormat = FHarmonyCompressionFormat{};
    }
};

// P11: how a section of a variant option lists the points it changes.
UENUM()
enum class EHarmonyVariantPointSetMode : uint8
{
    None,          // the section is absent
    All,           // every point of the asset, in order (no data)
    Bitmask,       // one bit per asset point (bit i of byte i / 8), listed points in ascending order
    DeltaVarint    // ascending indices as LEB128 varints of (index - previous index); the first is the index itself
};

USTRUCT()
struct HARMONY_API FHarmonyVariantPointSet
{
    GENERATED_BODY()

    UPROPERTY()
    EHarmonyVariantPointSetMode Mode = EHarmonyVariantPointSetMode::None;

    // Points listed.
    UPROPERTY()
    int32 Count = 0;

    UPROPERTY()
    TArray<uint8> Data;

    // Stores ascending, unique indices in the smallest of the three encodings.
    void Set(const TArray<int32>& SortedIndices, int32 NumPoints);

    // The listed points, ascending. False (OutIndices empty) when the data is inconsistent or lists a point outside
    // [0, NumPoints).
    bool Get(int32 NumPoints, TArray<int32>& OutIndices) const;
};

// P11: how an option stores the view-dependent part of its SH delta (coefficients 1..15).
UENUM()
enum class EHarmonyVariantSHRestMode : uint8
{
    None,           // the option keeps the base's view-dependent terms
    Int8,           // 45 signed bytes per listed point, value = byte * SHRestScale
    BandCodebooks,  // per SH band a codebook of delta vectors and one (or more: residual stages) index per listed point
    // The same with the delta split into grey and colour: per band a "luma" codebook (per coefficient the mean of R, G, B)
    // and a "chroma" codebook (per coefficient R - mean and B - mean; G follows). SHBands holds six entries: the three
    // bands' luma, then the three bands' chroma. A lighting change is mostly grey, so the luma vectors (3 / 5 / 7 numbers)
    // carry most of it.
    BandCodebooksLumaChroma
};

// P11: codebooks an SH-rest mode stores (0 for the modes without codebooks) and the vector length of codebook Index.
FORCEINLINE int32 GetHarmonyVariantSHCodebookCount(const EHarmonyVariantSHRestMode Mode)
{
    return Mode == EHarmonyVariantSHRestMode::BandCodebooks ? 3 : (Mode == EHarmonyVariantSHRestMode::BandCodebooksLumaChroma ? 6 : 0);
}

FORCEINLINE int32 GetHarmonyVariantSHCodebookDim(const EHarmonyVariantSHRestMode Mode, const int32 Index)
{
    static const int32 Coefficients[3] = { 3, 5, 7 };
    const int32 Band = ((Index % 3) + 3) % 3;
    if (Mode == EHarmonyVariantSHRestMode::BandCodebooksLumaChroma)
    {
        return Index < 3 ? Coefficients[Band] : Coefficients[Band] * 2;
    }
    return Coefficients[Band] * 3;
}

// P11: one SH band (1: coefficients 1-3, 2: 4-8, 3: 9-15) of an option stored as codebook indices.
USTRUCT()
struct HARMONY_API FHarmonyVariantSHBand
{
    GENERATED_BODY()

    // Entries per stage (1..65536). An index is 1 byte when <= 256, else 2 bytes (little endian).
    UPROPERTY()
    int32 NumEntries = 0;

    // Residual stages: a point's delta is the sum of one entry per stage. 0 = the band is unchanged.
    UPROPERTY()
    int32 NumStages = 0;

    // Codebook value = signed byte * Scale.
    UPROPERTY()
    float Scale = 0.0f;

    // NumStages * NumEntries * Dim signed bytes. Dim: coefficients of the band * 3 (per coefficient R, G, B); for a luma
    // codebook the coefficients of the band, for a chroma codebook twice that (per coefficient R - mean, B - mean).
    UPROPERTY()
    TArray<uint8> Codebook;

    // Per listed point, per stage, one index.
    UPROPERTY()
    TArray<uint8> Indices;
};

// Phase 13: the lighting of the captured scene, for lighting what is put into the splat environment afterwards (a vehicle).
// Taken during the Harmony capture from the rig's centre: a 360 degree HDR capture of the scene in absolute radiance (the
// scene's own units, independent of exposure). The asset has one for its base look and each variant option can have its
// own (time of day changes the lighting). UHarmonyLightingComponent (on AHarmonyActor) lights the level with it through a
// sky light. Empty Cubemap: no probe, nothing happens.
USTRUCT(BlueprintType)
struct HARMONY_API FHarmonyLightingProbe
{
    GENERATED_BODY()

    // The probe: a cubemap inside this asset's package (long-lat HDR source, as an imported .hdr).
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    TObjectPtr<UTextureCube> Cubemap;

    // What the cubemap's texels are multiplied by to get scene radiance (the scene's own units). The probe is stored
    // exposure-normalised, as a photograph taken at the capture's exposure (texel = radiance / (1.2 * 2^CaptureEV100)),
    // so that it reads as a picture in the texture editor and in other programs; the sky light's intensity carries this
    // factor, so the light is the same. 1: a probe stored as radiance itself (the first probes were).
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    float RadianceScale = 1.0f;

    // Width of the probe's long-lat source image (the height is half). The sky light's cube follows it: a quarter of it
    // a face.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    int32 Width = 0;

    // The exposure the capture's images (and so the splats) were rendered at. A level shows lit meshes at the brightness
    // the splats have only at this exposure: UHarmonyLightingComponent sets it for the level (its Exposure setting).
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    bool bHasCaptureEV100 = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting", meta = (EditCondition = "bHasCaptureEV100"))
    float CaptureEV100 = 0.0f;

    // Where the probe was taken (world, cm, in the captured level).
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    FVector CaptureLocation = FVector::ZeroVector;

    // The captured level's sun (its brightest directional light) as it arrives at the ground, that is, after the sky
    // atmosphere when the light is the atmosphere's sun: a directional light with SunIlluminance and SunColor, no
    // temperature and Atmosphere Sun Light off, lights like it. The cubemap holds the sky and the surroundings, with the
    // sun disc clamped (a sky light cannot stand in for a sun).
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    bool bHasSun = false;

    // Unit vector from the scene towards the sun (world).
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting", meta = (EditCondition = "bHasSun"))
    FVector SunDirection = FVector::UpVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting", meta = (EditCondition = "bHasSun"))
    FLinearColor SunColor = FLinearColor::White;

    // Lux at the ground: the directional light's intensity times what its colour and the atmosphere leave of it.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting", meta = (EditCondition = "bHasSun"))
    float SunIlluminance = 0.0f;

    // Phase 14: the sun's share of the light on the ground around the stand (direct sun over all light, per channel),
    // measured at capture. A shadow of the sun takes this much of the light away; the rest is the sky's.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    FLinearColor SunFraction = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Phase 14: whether the sun reaches the stand at all (no: what stands there is in the shade already and gets no sun
    // shadow of its own).
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    bool bSunReachesStand = false;

    bool IsValid() const { return Cubemap != nullptr; }
};

// Phase 14: what the shadows of things put into the environment fall on: the ground around the stand as a heightfield,
// taken at capture from above (below a height cap, so crowns and roofs are not in it). One per asset: variants share it.
USTRUCT(BlueprintType)
struct HARMONY_API FHarmonyShadowProxy
{
    GENERATED_BODY()

    // Size x Size heights. Column i is at X = Center.X - HalfExtent + (i + 0.5) / Size * 2 * HalfExtent, row j the same in Y.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    int32 Size = 0;

    // The stand (the capture rig's origin), in the asset's space (cm).
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    FVector Center = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    float HalfExtent = 0.0f;

    // height (cm, the asset's space) = MinHeight + value / 65535 * HeightRange
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    float MinHeight = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    float HeightRange = 0.0f;

    // Size * Size 16 bit values, row by row, low byte first.
    UPROPERTY()
    TArray<uint8> Heights;

    bool IsValid() const { return Size > 1 && HalfExtent > 0.0f && Heights.Num() == Size * Size * 2; }
};

// Phase 16: the floor's material around the stand, for reflecting what is put into the environment (a vehicle) in a shiny
// floor (notes/phase16_floor_reflections.md). Taken at capture from above over the shadow proxy's square (GBuffer
// roughness, specular and metallic), with the plane the reflection mirrors about, fitted at the stand. One per asset:
// variants share it (the floor's material does not change with the time of day). Empty: no reflections (the capture's
// Floor Reflection setting was Never, or Auto found the floor too rough to show one).
USTRUCT(BlueprintType)
struct HARMONY_API FHarmonyReflectionProxy
{
    GENERATED_BODY()

    // Size x Size texels in the shadow proxy's layout: column i along +X, row j along +Y.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    int32 Size = 0;

    // The stand (the capture rig's origin), in the asset's space (cm).
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    FVector Center = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    float HalfExtent = 0.0f;

    // The mirror plane (the asset's space): through Center + (0, 0, PlaneHeight - Center.Z) with this unit normal.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    float PlaneHeight = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    FVector PlaneNormal = FVector::UpVector;

    // What the bake measured near the stand (for the record): the share of the floor that shows a clear reflection,
    // weighted by its reflectance (FHarmonyReflectionProxy is only stored when it was enough, or when told to).
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    float ShinyShare = 0.0f;

    // The floor's mean base colour near the stand (A 0: not measured): the hidden floor that lets Lumen light a vehicle from
    // below as the real floor does (UHarmonyShadowCatcherComponent, Lumen Floor) takes it.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    FLinearColor FloorAlbedo = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // The floor's mean radiance near the stand seen from straight above (its diffuse level, about: scene radiance; 0: not
    // measured). The reflection pass never dims the floor below it.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    FLinearColor FloorRadiance = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Size * Size RGBA8 texels, row by row: R roughness, G specular, B metallic (the GBuffer's 0..1 values), A 255 where the
    // texel is the floor (within a few cm of the plane), 0 elsewhere (a wall, a step, an object: nothing is mirrored there).
    UPROPERTY()
    TArray<uint8> Material;

    bool IsValid() const { return Size > 1 && HalfExtent > 0.0f && Material.Num() == Size * Size * 4 && !PlaneNormal.IsNearlyZero(); }
};

// P10 configurable variants: one option of a variant set, stored as a sparse appearance delta on the asset's base splats.
// Geometry (position, rotation, scale, opacity) is shared with the base; an option replaces the full SH (16 coefficients x
// RGB, raw SH: coefficient 0 is the DC term) of the points it lists. Points not listed keep the base appearance.
USTRUCT(BlueprintType)
struct HARMONY_API FHarmonySplatVariantOption
{
    GENERATED_BODY()

    // Matches the Variant Manager variant (or any variant source's option) name.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gaussian Splat|Variants")
    FName Name;

    // Asset point indices this option changes, ascending.
    UPROPERTY()
    TArray<int32> PointIndices;

    // PointIndices.Num() * 48 half floats (FFloat16 bit patterns): per point, coefficients 0..15, each R, G, B.
    UPROPERTY()
    TArray<uint16> SHHalf;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Variants")
    int32 NumChangedPoints = 0;

    // Informational: how the option was baked (thresholds, PSNR, source caches).
    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Variants")
    FString BakeInfo;

    // Phase 13: the scene's lighting with this option shown (empty: the asset's own probe applies).
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gaussian Splat|Variants")
    FHarmonyLightingProbe LightingProbe;

    // UHarmonyDataAsset::PointSetId of the splats this option was baked against (invalid: baked before the id existed,
    // only the index range is checked).
    UPROPERTY()
    FGuid PointSetId;


    // ---- P11 sections (notes/phase11_variants.md). FormatVersion 0: a P10 option (PointIndices + SHHalf above, and nothing
    // below). 2: the sections below; each is optional (its point set's Mode is None when absent) and lists its own points.
    // All payloads are byte arrays (bulk-serialised); HarmonyVariantCodec reads them.
    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Variants")
    uint8 FormatVersion = 0;

    // Appearance: the SH of the listed points. DC: 3 signed bytes per listed point, value = byte * SHDCScale.
    UPROPERTY()
    FHarmonyVariantPointSet SHPoints;

    // false: the int8 values (SHDC, and SHRest in Int8 mode) are deltas to add to the base's SH. true: they are the SH
    // itself (baked on a compressed base, on the base's own int8 grids: nothing is lost that the base does not lose too).
    // Codebook entries are always deltas.
    UPROPERTY()
    bool bSHAbsolute = false;

    UPROPERTY()
    float SHDCScale = 0.0f;

    UPROPERTY()
    TArray<uint8> SHDC;

    UPROPERTY()
    EHarmonyVariantSHRestMode SHRestMode = EHarmonyVariantSHRestMode::None;

    UPROPERTY()
    float SHRestScale = 0.0f;

    UPROPERTY()
    TArray<uint8> SHRest;

    UPROPERTY()
    TArray<FHarmonyVariantSHBand> SHBands;

    // With codebooks: the listed points whose view-dependent delta is stored exactly instead (the points where the
    // codebooks' error would show most): 45 signed bytes each, value = byte * SHExactScale. A subset of SHPoints.
    UPROPERTY()
    FHarmonyVariantPointSet SHExactPoints;

    UPROPERTY()
    float SHExactScale = 0.0f;

    UPROPERTY()
    TArray<uint8> SHExact;

    // Opacity: the listed points' new opacity, one byte each (0..255 = 0..1). 0: the base splat is not part of this option.
    UPROPERTY()
    FHarmonyVariantPointSet OpacityPoints;

    UPROPERTY()
    TArray<uint8> Opacity;

    // Position: the delta to the base position (asset frame and units), 3 int16 (little endian) per listed point,
    // value = int16 * PositionScale.
    UPROPERTY()
    FHarmonyVariantPointSet PositionPoints;

    UPROPERTY()
    float PositionScale = 0.0f;

    UPROPERTY()
    TArray<uint8> Position;

    // Shape (rotation + scale): the listed points' new covariance, 7 half floats (little endian) each: the trace-normalised
    // covariance xx, xy, xz, yy, yz, zz and the log size 0.5 ln(trace). The base's own compressed covariance layout.
    UPROPERTY()
    FHarmonyVariantPointSet ShapePoints;

    UPROPERTY()
    TArray<uint8> Shape;

    // Size: one isotropic scale factor for a listed point whose shape otherwise stays the base's: the delta of its log
    // size (0.5 ln of the covariance's trace), 1 signed byte per listed point, value = byte * SizeScale.
    UPROPERTY()
    FHarmonyVariantPointSet SizePoints;

    UPROPERTY()
    float SizeScale = 0.0f;

    UPROPERTY()
    TArray<uint8> Size;

    // Splats only this option has: a self-contained chunk in the base's own encoding, drawn with the component's transform.
    UPROPERTY()
    FHarmonyRawData Added;

    // Stored size of the option (all sections), for the details panel.
    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Variants")
    float StoredSizeMB = 0.0f;

    static constexpr int32 HalfsPerPoint = 48;

    // P11: cheap size checks of the sections (the listed indices themselves are checked when the option is decoded).
    bool AreSectionsConsistent(int32 NumPoints) const;

    bool IsValidFor(int32 NumPoints, const FGuid& AssetPointSetId = FGuid()) const
    {
        if (PointSetId.IsValid() && PointSetId != AssetPointSetId)
        {
            return false;   // baked against other splats (the asset was retrained, decimated or reimported since)
        }
        if (FormatVersion != 0)
        {
            return AreSectionsConsistent(NumPoints);
        }
        if (SHHalf.Num() != PointIndices.Num() * HalfsPerPoint)
        {
            return false;
        }
        for (const int32 Index : PointIndices)
        {
            if (Index < 0 || Index >= NumPoints)
            {
                return false;
            }
        }
        return true;
    }
};

// A variant set (slot): independent options of one part of the scene (wall finish, car paint, ...). The base option is
// the appearance the asset's own splats already have (it needs no data).
USTRUCT(BlueprintType)
struct HARMONY_API FHarmonySplatVariantSet
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gaussian Splat|Variants")
    FName Name;

    // The option the base splats show (selecting it, or nothing, applies no delta).
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gaussian Splat|Variants")
    FName BaseOption;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Variants")
    TArray<FHarmonySplatVariantOption> Options;

    const FHarmonySplatVariantOption* FindOption(FName OptionName) const
    {
        return Options.FindByPredicate([OptionName](const FHarmonySplatVariantOption& O) { return O.Name == OptionName; });
    }
};

// How a fallback mesh shows one variant option (Docs/Dev/SplatToMesh_M4_Variants.md).
UENUM(BlueprintType)
enum class EHarmonyFallbackOptionKind : uint8
{
    // The option changes appearance only: the tier's base mesh with one material per material slot (own atlas pages, and
    // for a glossy floor its own cubemap). No geometry is stored.
    SharedGeometry UMETA(DisplayName = "Shared Geometry (materials only)"),
    // The option changes geometry: a complete mesh of its own, with its own materials.
    OwnMesh UMETA(DisplayName = "Own Mesh"),
    // Reserved, not built yet: the base mesh with the sections in HiddenBaseSlots hidden, PatchMesh drawn in their place
    // (the rebuilt region around ChangedBounds) and Materials on the slots that stay.
    Patch UMETA(DisplayName = "Patch (reserved)")
};

// One variant option of one fallback tier. Keyed by (VariantSet, Option), the names the splat's variant sets use
// (FHarmonySplatVariantSet::Name, FHarmonySplatVariantOption::Name).
USTRUCT(BlueprintType)
struct HARMONY_API FHarmonyFallbackOption
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fallback")
    FName VariantSet;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fallback")
    FName Option;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fallback")
    EHarmonyFallbackOptionKind Kind = EHarmonyFallbackOptionKind::SharedGeometry;

    // SharedGeometry (and Patch): the material of every material slot of the tier's base mesh, in slot order.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fallback")
    TArray<TSoftObjectPtr<UMaterialInterface>> Materials;

    // OwnMesh: the option's mesh (its materials are its own).
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fallback")
    TSoftObjectPtr<UStaticMesh> Mesh;

    // Patch (reserved): the rebuilt region and the base mesh's material slots it replaces.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fallback", AdvancedDisplay)
    TSoftObjectPtr<UStaticMesh> PatchMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fallback", AdvancedDisplay)
    TArray<FName> HiddenBaseSlots;

    // What the build's change detection found against the base capture (same poses): where the surface was added or
    // removed (Unreal world space, cm; invalid when nothing changed), the share of compared depth pixels that changed and
    // the share of image pixels that changed. Kept for the patch path and for flagging options that overlap.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fallback")
    FBox ChangedBounds = FBox(ForceInit);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fallback")
    float ChangedDepthFraction = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fallback")
    float ChangedImageFraction = 0.0f;

    // Texture memory the option adds on top of the base mesh, in MB: its atlas pages (BC7, 1 byte per texel) and cubemap
    // (BC6H) with full mip chains. For budgeting.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fallback")
    float TextureMB = 0.0f;
};

// The fallback of one tier: the base mesh and the options built for it.
USTRUCT(BlueprintType)
struct HARMONY_API FHarmonyFallbackTierAssets
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fallback")
    TSoftObjectPtr<UStaticMesh> Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fallback")
    TArray<FHarmonyFallbackOption> Options;

    const FHarmonyFallbackOption* FindOption(FName VariantSet, FName Option) const
    {
        return Options.FindByPredicate([VariantSet, Option](const FHarmonyFallbackOption& O) { return O.VariantSet == VariantSet && O.Option == Option; });
    }
};

UCLASS()
class HARMONY_API UHarmonyDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat")
    FHarmonyRawData SplatData;

    // Phase 13: the captured scene's lighting (base look), used by UHarmonyLightingComponent. Empty: none was captured.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    FHarmonyLightingProbe LightingProbe;

    // Phase 14: the ground the shadows of things put into the environment fall on (UHarmonyShadowCatcherComponent).
    // Empty: a plane is used.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    FHarmonyShadowProxy ShadowProxy;

    // Phase 16: the floor's material, for reflecting things put into the environment in it (UHarmonyShadowCatcherComponent,
    // Catch Reflections). Empty: no reflections.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighting")
    FHarmonyReflectionProxy ReflectionProxy;

    // Fallback meshes for devices that cannot run the splat renderer, by tier name ("Low", "Mid", "High"): simplified,
    // textured, unlit static meshes built from the same capture (HarmonyFallbackEditor), each with the variant options
    // built for it. Soft references: nothing loads or shows them at runtime yet (the runtime switch waits for a device
    // test); UHarmonyFallbackLibrary::ApplyFallbackOption shows an option on a placed actor in the editor.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fallback")
    TMap<FName, FHarmonyFallbackTierAssets> FallbackTiers;

    const FHarmonyFallbackTierAssets* FindFallbackTier(FName TierName) const { return FallbackTiers.Find(TierName); }

    // The probe that goes with ActiveVariants: the one of the last variant set (in the asset's order) whose active option
    // has a probe, else the asset's own. Null: no probe.
    const FHarmonyLightingProbe* FindLightingProbe(const TMap<FName, FName>& ActiveVariants) const;

    // P10: configurable variants baked on top of SplatData (sparse appearance deltas). Empty for a plain splat.
    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Variants")
    TArray<FHarmonySplatVariantSet> VariantSets;

    const FHarmonySplatVariantSet* FindVariantSet(FName SetName) const
    {
        return VariantSets.FindByPredicate([SetName](const FHarmonySplatVariantSet& S) { return S.Name == SetName; });
    }

    // P10: identifies SplatData's point set (count and order). Variant options record it when baked and are ignored when it
    // no longer matches. Changed by OnSplatDataReplaced; kept by RemapVariantPoints.
    UPROPERTY(VisibleAnywhere, AdvancedDisplay, Category = "Gaussian Splat|Variants")
    FGuid PointSetId;

    // Call after SplatData is replaced by other splats (retrained, decimated, reimported): a new PointSetId, and the variant
    // sets - baked for the old splats - are removed (logged with Why).
    void OnSplatDataReplaced(const TCHAR* Why);

    // Call after SplatData's points are reordered (not changed): NewIndexOfOld[old point index] = new index. Keeps every
    // variant option valid by remapping its points (rows re-sorted by the new index).
    void RemapVariantPoints(const TArray<int32>& NewIndexOfOld);

    // Changes whenever the renderer must re-upload this asset although its path and counts did not change: a new value per
    // instance (package reload) and on every edit (PostEditChange, undo/redo, OnSplatDataReplaced, RemapVariantPoints).
    // Code that changes SplatData or VariantSets in place without PostEditChange should call BumpRenderRevision.
    uint32 GetRenderRevision() const { return RenderRevision; }
    void BumpRenderRevision();

    //~ UObject interface
    virtual void PostInitProperties() override;
#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
    // Exposes key splat metadata (point count, encoding, data size) as Asset Registry tags so they
    // appear immediately in the Content Browser hover tooltip for all assets — including ones created
    // in-session via duplication, which have not yet been scanned from disk by the Asset Registry.
    virtual void GetAssetRegistryTags(FAssetRegistryTagsContext Context) const override;
    //~ End UObject interface

private:
    // Deliberately not a UPROPERTY: neither saved nor restored by a transaction (undo must change it, not roll it back).
    uint32 RenderRevision = 0;
};
