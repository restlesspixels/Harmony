#pragma once

#include "CoreMinimal.h"
#include "HarmonyDataAsset.h"          // complete type for TWeakObjectPtr<UHarmonyDataAsset>
#include "CoreGlobals.h"               // IsInGameThread / IsInRenderingThread (CoreGlobals.h:719, :739)
#include "RHIResources.h"
#include "UObject/WeakObjectPtr.h"

/**
 * GPU buffers in Harmony's Expanded per-splat layout (the FHarmonyRawData::PosRadius / Color / Covariance / SHCoeffs
 * arrays as Harmony_UploadSplatData uploads them), owned by someone else. Render thread only.
 * - PosRadius  StructuredBuffer<float4> NumRows: (component-local cm, 1)
 * - Color      StructuredBuffer<float4> NumRows: (0, 0, 0, opacity); rows with opacity 0 are culled by the preprocess
 * - Covariance StructuredBuffer<float4> 2*NumRows: (xx, xy, xz, yy), (yz, zz, 0, 0), cm^2
 * - SHCoeffs   StructuredBuffer<float4> 16*NumRows: (r, g, b, 0) per coefficient
 * The buffers must be readable as SRVs (ERHIAccess::SRVMask) whenever Harmony may draw. Generation must change whenever
 * the contents change: it is folded into Harmony's static-view preprocess cache key.
 */
struct FHarmonyExternalExpandedBuffers
{
	FShaderResourceViewRHIRef PosRadius;
	FShaderResourceViewRHIRef Color;
	FShaderResourceViewRHIRef Covariance;
	FShaderResourceViewRHIRef SHCoeffs;
	uint32 NumRows = 0;
	uint32 SHDegree = 3;
	uint64 Generation = 0;

	bool IsValid() const
	{
		return NumRows > 0 && PosRadius.IsValid() && Color.IsValid() && Covariance.IsValid() && SHCoeffs.IsValid();
	}
};

/**
 * Zero-copy external splat source (HarmonyTraining Phase 4 / P3 live preview, CONTRACT_P4.md section 3).
 *
 * A transient Expanded placeholder asset (NumRows rows, opacity 0, UnitScale 100) shown by a UHarmonyComponent reserves
 * the rows: Harmony uploads it once and sizes its instance, sort and preprocess buffers from it as usual. While the
 * source is registered (UHarmonySubsystem::RegisterExternalSplatSource) and the placeholder is the only Expanded Gaussian
 * source of the rendered world, Harmony's expanded preprocess reads the external buffers instead of the placeholder's
 * uploaded ones (same row count required), so the external splats are sorted, culled and composited with the scene
 * exactly like an asset, with no CPU copy. Compressed and billboard assets of the same world are unaffected. Any other
 * Expanded asset in the world disables the substitution (the placeholder's empty rows are then drawn, i.e. nothing).
 * While registered, Harmony sorts on the GPU (the CPU sorter only knows the placeholder's positions).
 *
 * Threading: the placeholder is a game-thread value; the buffers are set and read on the render thread only.
 */
class FHarmonyExternalSplatSource : public TSharedFromThis<FHarmonyExternalSplatSource, ESPMode::ThreadSafe>
{
public:
	explicit FHarmonyExternalSplatSource(UHarmonyDataAsset* InPlaceholder)
		: Placeholder(InPlaceholder)
	{
	}

	/** Game thread. */
	UHarmonyDataAsset* GetPlaceholder_GameThread() const
	{
		check(IsInGameThread());
		return Placeholder.Get();
	}

	/** Render thread. An invalid (empty) value makes Harmony fall back to the placeholder's rows. */
	void SetBuffers_RenderThread(const FHarmonyExternalExpandedBuffers& InBuffers)
	{
		check(IsInRenderingThread());
		Buffers = InBuffers;
	}

	void ResetBuffers_RenderThread()
	{
		check(IsInRenderingThread());
		Buffers = FHarmonyExternalExpandedBuffers();
	}

	const FHarmonyExternalExpandedBuffers& GetBuffers_RenderThread() const
	{
		check(IsInRenderingThread());
		return Buffers;
	}

private:
	TWeakObjectPtr<UHarmonyDataAsset> Placeholder;
	FHarmonyExternalExpandedBuffers Buffers;
};
