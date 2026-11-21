#pragma once

#include "CoreMinimal.h"
#include "HarmonyDataAsset.h"

// P11 (configurable variants, sectioned options): the one place that reads an option's stored bytes. The renderer's upload,
// the editor's export and the evaluation all go through Decode + the Apply functions, so what is measured is what is shown.
namespace HarmonyVariant
{
    inline constexpr int32 SHFloatsPerPoint = 48;          // 16 coefficients x RGB, [k][c], coefficient 0 = DC
    inline constexpr int32 SHRestFloatsPerPoint = 45;
    inline constexpr int32 ShapeFloatsPerPoint = 7;        // trace-normalised covariance xx, xy, xz, yy, yz, zz + log size (0.5 ln trace)
    inline constexpr int32 NumSHBands = 3;
    HARMONY_API int32 SHBandFirstCoefficient(int32 Band);  // 1, 4, 9
    HARMONY_API int32 SHBandNumCoefficients(int32 Band);   // 3, 5, 7

    // One option decoded into flat arrays. Point lists are ascending. Values are in the asset's own frame and units.
    struct HARMONY_API FDecoded
    {
        // Appearance: 48 floats per listed point: the SH itself where the flag says absolute (DC: coefficient 0, rest:
        // coefficients 1..15), else the delta to add to the base's decoded SH.
        TArray<int32> SHPoints;
        TArray<float> SH;
        bool bDCAbsolute = false;
        bool bRestAbsolute = false;
        // Opacity: the new opacity, 0..1 (0 = the base splat is not in this option).
        TArray<int32> OpacityPoints;
        TArray<float> Opacity;
        // Position: the delta to add to the base position.
        TArray<int32> PositionPoints;
        TArray<FVector3f> PositionDelta;
        // Shape: the new covariance, ShapeFloatsPerPoint floats per listed point.
        TArray<int32> ShapePoints;
        TArray<float> Shape;
        // Size: the delta of the log size (the covariance is multiplied by exp(2 * delta)).
        TArray<int32> SizePoints;
        TArray<float> SizeDelta;

        bool IsEmpty() const
        {
            return SHPoints.Num() == 0 && OpacityPoints.Num() == 0 && PositionPoints.Num() == 0 && ShapePoints.Num() == 0 && SizePoints.Num() == 0;
        }
    };

    // False when the option's data is inconsistent (wrong sizes, indices outside [0, NumPoints)); Out is then empty.
    HARMONY_API bool Decode(const FHarmonySplatVariantOption& Option, int32 NumPoints, FDecoded& Out);

    // Expanded arrays of NumPoints points (FHarmonyRawData's Expanded layout: PosRadius, Color (w = opacity), 2 covariance
    // float4 per point, SHStride float4 per point). NumCoeffs: SH coefficients in use (the rest stay as they are).
    HARMONY_API void ApplyToExpanded(const FDecoded& Decoded, int32 NumPoints, int32 SHStride, int32 NumCoeffs,
        FVector4f* PosRadius, FVector4f* Color, FVector4f* Covariance, FVector4f* SH);

    // The renderer's upload arrays of one Compressed source (as HarmonyViewExtension builds them): Metadata = 3 words per
    // point (word 0: opacity byte, size byte; word 1: covariance entry; word 2: SH feature), Positions = decoded float3 per
    // point, CovarianceWords / SHWords = the whole upload arrays (3 / 12 words per entry). Values are re-quantised with the
    // base asset's own quantisers, exactly as the shaders dequantise them. A size the base's size byte cannot express
    // exactly goes into the (half float) covariance words, so shapes and sizes lose nothing to the byte. bPatchShape false
    // (the base shares covariance entries between points): shape and size are left alone.
    HARMONY_API void ApplyToCompressedUpload(const FDecoded& Decoded, const FHarmonyRawData& Base, int32 NumPoints, uint32* Metadata,
        FVector3f* Positions, uint32* CovarianceWords, int32 NumCovarianceEntries, uint32* SHWords, int32 NumSHFeatures, bool bPatchShape);

    // One option's rows in the upload encoding of its Compressed base: exactly the values ApplyToCompressedUpload writes,
    // kept so that a switch copies rows instead of decoding the option again (the renderer caches one per option). Sparse:
    // only the listed points; point lists ascend. This is also what a second GPU buffer next to the base would hold for
    // an animated transition between the base and the option.
    struct HARMONY_API FCompressedPatch
    {
        TArray<int32> SHPoints;
        TArray<uint32> SHWords;            // 12 words (48 signed bytes) per SH point
        TArray<int32> OpacityPoints;
        TArray<uint8> OpacityBytes;        // byte 0 of metadata word 0
        TArray<int32> PositionPoints;
        TArray<FVector3f> Positions;       // the position itself
        TArray<int32> GeometryPoints;      // the shape and size sections' points
        TArray<uint32> CovarianceWords;    // 3 words per geometry point
        TArray<uint8> SizeBytes;           // byte 1 of metadata word 0, per geometry point

        // The base's own values of the same rows (what the option replaces), same layout: the "undo" rows of a fade away
        // from this option once it is baked into the main buffers.
        TArray<uint32> BaseSHWords;
        TArray<uint8> BaseOpacityBytes;
        TArray<FVector3f> BasePositions;
        TArray<uint32> BaseCovarianceWords;
        TArray<uint8> BaseSizeBytes;

        int64 GetAllocatedBytes() const;
    };

    // Runs ApplyToCompressedUpload on the base's upload arrays (same arguments), records every row it wrote in OutPatch and
    // puts the arrays back as they were. The arrays must hold the base alone (no other option applied).
    HARMONY_API void BuildCompressedPatch(const FDecoded& Decoded, const FHarmonyRawData& Base, int32 NumPoints, uint32* Metadata,
        FVector3f* Positions, uint32* CovarianceWords, int32 NumCovarianceEntries, uint32* SHWords, int32 NumSHFeatures, bool bPatchShape,
        FCompressedPatch& OutPatch);

    // Writes a patch's rows into upload arrays laid out like the ones it was built from (the entry and feature indices in
    // Metadata may be offset differently). Of two patches that list the same point, the one applied last wins.
    HARMONY_API void ApplyCompressedPatch(const FCompressedPatch& Patch, int32 NumPoints, uint32* Metadata, FVector3f* Positions,
        uint32* CovarianceWords, int32 NumCovarianceEntries, uint32* SHWords, int32 NumSHFeatures);

    // ---- fade slots (animated variant transitions) ----
    // A slot is what the renderer puts on the GPU next to the main buffers while a fade runs: the rows of one look (one
    // option, or the options of several sets merged), or the base's rows under a look. One fixed-size row per listed point:
    //   words 0..2  position (float bits)
    //   word 3      byte 0 opacity, byte 1 size, byte 2 the sections the row has (flags below)
    //   words 4..6  covariance words
    //   words 7..18 SH words
    // A section a row does not have is "as the base". RowOfPoint maps a point to its row.
    inline constexpr int32 SlotRowWords = 19;
    inline constexpr uint32 SlotNoRow = 0xFFFFFFFFu;
    inline constexpr uint32 SlotRowHasSH = 1u;
    inline constexpr uint32 SlotRowHasOpacity = 2u;
    inline constexpr uint32 SlotRowHasPosition = 4u;
    inline constexpr uint32 SlotRowHasGeometry = 8u;

    struct HARMONY_API FCompressedSlotRows
    {
        int32 NumPoints = 0;
        int32 NumRows = 0;
        TArray<uint32> RowOfPoint;         // NumPoints entries, SlotNoRow = the point has no row
        TArray<uint32> Rows;               // SlotRowWords per row
    };

    // Rows of the points the patches list (any section). Cheap: one pass over the point lists.
    HARMONY_API int32 CountCompressedSlotRows(TConstArrayView<TSharedPtr<const FCompressedPatch>> Patches, int32 NumPoints);
    HARMONY_API int64 GetCompressedSlotGpuBytes(int32 NumPoints, int32 NumRows);

    // Patches in set order: where two list the same point and section the later one wins, as in the upload. bBaseRows:
    // the base's values of those rows instead of the options'. Touches no UObject: safe on a worker thread.
    HARMONY_API void BuildCompressedSlotRows(TConstArrayView<TSharedPtr<const FCompressedPatch>> Patches, bool bBaseRows, int32 NumPoints,
        FCompressedSlotRows& Out);

    // ---- encoding helpers (used by the editor's bake) ----
    HARMONY_API void EncodeHalf(float Value, uint8* Dest);       // 2 bytes, little endian
    HARMONY_API float DecodeHalf(const uint8* Src);

    // Stored bytes of an option (every section, the added chunk included).
    HARMONY_API int64 GetStoredBytes(const FHarmonySplatVariantOption& Option);
    // "SH 5.1 MB (dense, codebooks 3 x 4096), opacity 0.2 MB (213,004 points), ..." for logs and BakeInfo.
    HARMONY_API FString Describe(const FHarmonySplatVariantOption& Option);
}
