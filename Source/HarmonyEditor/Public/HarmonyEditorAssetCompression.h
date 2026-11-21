#pragma once

#include "CoreMinimal.h"

class UHarmonyDataAsset;
struct FHarmonyRawData;

// Public, exported half of HarmonyAssetCompression.h (HarmonyTraining Phase 4, CONTRACT_P4.md P1): other editor modules
// (HarmonyTrainingEditor) compress assets they write with exactly the code the PLY factory and the "Convert to Compressed"
// action use. Private/HarmonyAssetCompression.h includes this header, so every existing include keeps working.
namespace Harmony::Editor
{
    // --- Compress (Expanded → Compressed) ---

    // Returns true if the asset can be compressed (Expanded encoding, valid payload).
    // Populates OutReason if provided and the check fails.
    HARMONYEDITOR_API bool CanCompressDataAsset(const UHarmonyDataAsset* DataAsset, FString* OutReason = nullptr);

    // Quantises and repacks the expanded splat data into the compressed GPU layout in-place.
    // Only call after CanCompressDataAsset returns true. Fails on a non-finite position ("Splat %d has a non-finite
    // position."): callers drop such splats first.
    HARMONYEDITOR_API bool CompressDataAssetInPlace(UHarmonyDataAsset& DataAsset, FString& OutError);

    // The same checks and compression on bare splat data, with no UObject calls (no Modify / MarkPackageDirty /
    // PostEditChange), so it may run on a worker thread on data nothing else touches (HarmonyTraining's async asset save).
    // CompressDataAssetInPlace(Asset) == CompressSplatDataInPlace(Asset.SplatData) + Modify/dirty/PostEditChange.
    HARMONYEDITOR_API bool CanCompressSplatData(const FHarmonyRawData& SplatData, FString* OutReason = nullptr);
    HARMONYEDITOR_API bool CompressSplatDataInPlace(FHarmonyRawData& SplatData, FString& OutError);

    // --- Decompress (Compressed -> Expanded) ---

    // Returns true if the asset can be expanded (Compressed encoding, non-sparse indices).
    // Sparse NPZ-imported assets (geometry_count < num_points) cannot be decompressed this way.
    // Populates OutReason if provided and the check fails.
    HARMONYEDITOR_API bool CanDecompressDataAsset(const UHarmonyDataAsset* DataAsset, FString* OutReason = nullptr);

    // Dequantises the compressed splat data back to float32 expanded layout in-place.
    // NOTE: This is a lossy round-trip - int8/float16 quantization error from the original
    // compress cannot be recovered. Only call after CanDecompressDataAsset returns true.
    HARMONYEDITOR_API bool DecompressDataAssetInPlace(UHarmonyDataAsset& DataAsset, FString& OutError);
}
