#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "HarmonyExternalSplatSource.h"
#include "RenderGraphFwd.h"
#include "RHIResources.h"
#include "ScreenPass.h"
#include "SceneViewExtension.h"
#include "UObject/StrongObjectPtr.h"

#include <atomic>

class FPostOpaqueRenderParameters;
class FHarmonyCpuSorter;
class UHarmonyDataAsset;
struct FHarmonySplatVariantOption;
class UHarmonyComponent;
namespace HarmonyVariant
{
	struct FCompressedPatch;
}
namespace HarmonyVariantFade
{
	struct FCommands;
	struct FSegment;
}

// The two stages of the splat composite. Early runs at post-opaque (before translucency) and handles
// pixels without opaque coverage; Late runs after Harmony's custom tonemap (both hook BeforeDOF, so
// splats still receive DOF) and composites the splat contribution over opaque-covered pixels.
enum class EHarmonySplatStage : uint8
{
	Early,
	Late
};

class HARMONY_API FHarmonyViewExtension : public FSceneViewExtensionBase
{
public:
    FHarmonyViewExtension(const FAutoRegister& AutoRegister);
    virtual ~FHarmonyViewExtension() override;

	void ReleaseSplatRenderResources_GameThread();

	// Zero-copy external splat source (HarmonyExternalSplatSource.h). Game thread; nullptr clears it.
	void SetExternalSplatSource_GameThread(TSharedPtr<FHarmonyExternalSplatSource, ESPMode::ThreadSafe> Source);
	const TSharedPtr<FHarmonyExternalSplatSource, ESPMode::ThreadSafe>& GetExternalSplatSource_GameThread() const { return ExternalSplatSource_GameThread; }

	//~ Begin FSceneViewExtensionBase Interface
	virtual bool IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const override;
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override;
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& View) override;
	virtual void PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs) override;
	virtual void SubscribeToPostProcessingPass(EPostProcessingPass Pass, const FSceneView& InView, FPostProcessingPassDelegateArray& InOutPassCallbacks, bool bIsPassEnabled) override;
	//~ End FSceneViewExtensionBase Interface

private:
	bool IsDirectDrawPreparedForView_RenderThread(const FSceneView& View) const;
	void EnsureDirectDrawPreparedForView_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View);
	void PrepareDirectDraw_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View);
	// Early splat stage: hooked at post-opaque; draws splats where the scene has no opaque coverage.
	void RenderEarlySplatsPass_RenderThread(FPostOpaqueRenderParameters& Parameters);
	// Commits the early stage's stored representative splat depth after UE standard translucency has
	// rendered, but before DOF / TSR consume SceneDepth. This keeps translucent shadow/modulate geometry
	// from being rejected against a splat depth that did not exist when UE rendered its opaque scene.
	void RenderDeferredBackgroundSceneDepthPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View);
	FScreenPassTexture RenderSplatsCommon_RenderThread(
		FRDGBuilder& GraphBuilder,
		const FSceneView& View,
		FScreenPassTexture SceneColor,
		FScreenPassRenderTarget Output,
		TRDGUniformBufferRef<FSceneTextureUniformParameters> SceneTexturesUniform,
		FRDGTextureRef SceneVelocityTexture,
		EHarmonySplatStage Stage = EHarmonySplatStage::Early,
		// When true, the draw target is a dedicated offscreen splat buffer (not scene color), so the
		// draw writes premultiplied color AND coverage to alpha (CW_RGBA) instead of preserving scene
		// alpha — required so the later upscale/composite can occlude the scene with the coverage.
		bool bRenderIntoDedicatedSplatBuffer = false);
	FScreenPassTexture RenderSceneCoverageMaskVisualizationPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
	FScreenPassTexture RenderSplatOverdrawVisualizationPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
	FScreenPassTexture RenderSeparateTranslucencyVisualizationPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
	FScreenPassTexture RenderCustomTonemapPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
	FScreenPassTexture RenderTonemapReplacementPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
	// UE's stock tonemapper does not reliably include Harmony's splat coverage
	// in MRQ alpha. Restore only that alpha after stock tonemapping while
	// leaving UE's final RGB untouched.
	FScreenPassTexture RenderStockTonemapAlphaRestorePass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
	// Late splat stage: composites after standard translucency at BeforeDOF. EarlyMasked runs this
	// after its custom tonemap (including PT's full-screen foreground tonemap); Stock and
	// TonemapCompensation run it before the stock UE tonemapper.
	FScreenPassTexture RenderLateSplatsPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
	void ReleaseSplatRenderResources_RenderThread(bool bPreservePublishedProxy = false);

	struct FRenderThreadState
	{
		struct FCompressedDispatchSegment
		{
			uint32 SourceSplatOffset = 0u;
			uint32 NumSplats = 0u;
			FIntVector4 QuantizationZeroPoints = FIntVector4(0, 0, 0, 0);
			FVector4f QuantizationScales = FVector4f(1.0f, 1.0f, 1.0f, 1.0f);
		};

		// PrePostProcessPass exposes the renderer's full translucency resource map,
		// including the modulation/transmittance targets that are not present in
		// FPostProcessMaterialInputs. Keep the RDG references only for their frame.
		struct FCapturedTranslucencyModulateResources
		{
			FRDGTextureRef PostDOFTexture = nullptr;
			FIntRect PostDOFViewRect = FIntRect(0, 0, 0, 0);
			FRDGTextureRef StandardTexture = nullptr;
			FIntRect StandardViewRect = FIntRect(0, 0, 0, 0);
		};

		struct FPostEarlySceneColorSnapshot
		{
			FRDGTextureRef Texture = nullptr;
			FIntRect ViewRect = FIntRect(0, 0, 0, 0);
		};

		struct FDeferredBackgroundSceneDepthWrite
		{
			FRDGTextureRef BackgroundDepthTexture = nullptr;
			FIntRect BackgroundDepthViewRect = FIntRect(0, 0, 0, 0);
			FRDGTextureRef OpaqueSceneDepthTexture = nullptr;
			FIntRect OpaqueSceneDepthViewRect = FIntRect(0, 0, 0, 0);
			FIntRect SceneDepthViewRect = FIntRect(0, 0, 0, 0);
			float CoverageThreshold = 0.0f;
			float OpaqueDepthDistance = 0.0f;
			float DepthBias = 0.0f;
			float DepthFeather = 0.0f;
			bool bUseOpaqueSceneMask = false;
			bool bNoOpaqueOnly = false;
			bool bWriteVelocity = false;
			// Commit the true-depth channel (G) instead of the offset depth (R).
			bool bWriteTrueDepth = false;
		};

		// Frame-local resources owned by one concrete FSceneView. Keeping these
		// together is the first boundary required for supporting multiple view
		// families without one view overwriting another view's compose inputs.
		struct FPerViewRenderState
		{
			uint64 FrameCounter = MAX_uint64;
			FCapturedTranslucencyModulateResources CapturedTranslucencyModulate;
			FPostEarlySceneColorSnapshot PostEarlySceneColorSnapshot;
			FDeferredBackgroundSceneDepthWrite DeferredBackgroundSceneDepthWrite;
			FRDGTextureRef OpaqueSceneDepthTexture = nullptr;
			FIntRect OpaqueSceneDepthViewRect = FIntRect(0, 0, 0, 0);
			// Original path-tracer output captured before Harmony's full-screen
			// tonemap. MRQ intermediate targets do not reliably retain empty-
			// background inverse opacity, so late splat ownership reads this copy.
			FRDGTextureRef PathTracingPreTonemapCoverageTexture = nullptr;
			FIntRect PathTracingPreTonemapCoverageViewRect = FIntRect(0, 0, 0, 0);
			// Final PT inverse opacity after Harmony has placed the complete
			// background proxy underneath the traced foreground. Stock UE
			// tonemapping may replace alpha, so its after-tonemap callback reads
			// this frame-local texture and restores only the coverage channel.
			FRDGTextureRef PathTracingComposedCoverageTexture = nullptr;
			FIntRect PathTracingComposedCoverageViewRect = FIntRect(0, 0, 0, 0);
			FRDGTextureRef SplatOverdrawCounterTexture = nullptr;
			FIntRect SplatOverdrawCounterViewRect = FIntRect(0, 0, 0, 0);
			FRDGTextureRef EarlyStageCoverageTexture = nullptr;
			FIntRect EarlyStageCoverageViewRect = FIntRect(0, 0, 0, 0);
			FRDGBufferSRVRef OpaqueTileMaskSRV = nullptr;
			FIntPoint OpaqueTileMaskTiles = FIntPoint::ZeroValue;
			UPTRINT SplatRenderTargetKey = 0u;
			FIntRect SplatRenderSourceViewRect = FIntRect(0, 0, 0, 0);
			FIntRect SplatRenderViewRect = FIntRect(0, 0, 0, 0);
			FIntPoint SplatRenderExtent = FIntPoint::ZeroValue;
			bool bHasCapturedTranslucencyModulate = false;
			bool bHasPostEarlySceneColorSnapshot = false;
			bool bHasDeferredBackgroundSceneDepthWrite = false;
		};

		// Persistent proxy resources/cache metadata belonging to one view-family
		// render target. The existing scalar fields below remain the active working
		// set while callbacks execute; switching views saves/loads this state.
		struct FProxyTargetState
		{
			FTextureRHIRef BackgroundSplatsTargetRHI;
			EPixelFormat BackgroundSplatsRequestedFormat = PF_Unknown;
			bool bBackgroundSplatsRequestedSRGB = false;
			FIntPoint BackgroundSplatsRequestedExtent = FIntPoint::ZeroValue;
			FTextureRHIRef BackgroundAverageDepthTargetRHI;
			EPixelFormat BackgroundAverageDepthRequestedFormat = PF_Unknown;
			bool bBackgroundAverageDepthRequestedSRGB = false;
			FIntPoint BackgroundAverageDepthRequestedExtent = FIntPoint::ZeroValue;
			FTextureRHIRef BackgroundAmbiguityResolveTargetRHI;
			EPixelFormat BackgroundAmbiguityResolveRequestedFormat = PF_Unknown;
			bool bBackgroundAmbiguityResolveRequestedSRGB = false;
			FIntPoint BackgroundAmbiguityResolveRequestedExtent = FIntPoint::ZeroValue;
			FTextureRHIRef BackgroundDirectCoverageTargetRHI;
			EPixelFormat BackgroundDirectCoverageRequestedFormat = PF_Unknown;
			bool bBackgroundDirectCoverageRequestedSRGB = false;
			FIntPoint BackgroundDirectCoverageRequestedExtent = FIntPoint::ZeroValue;
			bool bBackgroundSplatsTargetValid = false;
			bool bReuseBackgroundSplatsTargetThisFrame = false;
			uint32 BackgroundSplatsTargetDrawConfigHash = 0u;
			FIntRect BackgroundProxyRasterViewRect = FIntRect(0, 0, 0, 0);
			bool bBackgroundAverageDepthTargetValid = false;
			bool bReuseBackgroundAverageDepthTargetThisFrame = false;
			uint32 BackgroundAverageDepthTargetDrawConfigHash = 0u;
			bool bBackgroundAmbiguityResolveTargetValid = false;
			bool bReuseBackgroundAmbiguityResolveTargetThisFrame = false;
			uint32 BackgroundAmbiguityResolveTargetDrawConfigHash = 0u;
			bool bBackgroundDirectCoverageTargetValid = false;
			FIntRect BackgroundDirectCoverageViewRect = FIntRect(0, 0, 0, 0);
			FTextureRHIRef SceneCoverageHistoryTextures[2];
			FIntPoint SceneCoverageHistoryExtent = FIntPoint::ZeroValue;
			FIntRect SceneCoverageHistoryViewRect = FIntRect(0, 0, 0, 0);
			EPixelFormat SceneCoverageHistoryFormat = PF_Unknown;
			int32 SceneCoverageHistoryReadIndex = 0;
			bool bSceneCoverageHistoryValid = false;
			uint64 LastUsedRenderFrame = 0u;
			// DirectDrawViewCacheGeneration the proxy textures above were rendered for.
			uint64 ProxyContentCacheGeneration = 0u;
		};

		bool bEnabled = false;
		FTextureRHIRef BackgroundSplatsTargetRHI;
		EPixelFormat BackgroundSplatsRequestedFormat = PF_Unknown;
		bool bBackgroundSplatsRequestedSRGB = false;
		FIntPoint BackgroundSplatsRequestedExtent = FIntPoint::ZeroValue;
		FTextureRHIRef BackgroundAverageDepthTargetRHI;
		EPixelFormat BackgroundAverageDepthRequestedFormat = PF_Unknown;
		bool bBackgroundAverageDepthRequestedSRGB = false;
		FIntPoint BackgroundAverageDepthRequestedExtent = FIntPoint::ZeroValue;
			FTextureRHIRef BackgroundAmbiguityResolveTargetRHI;
			EPixelFormat BackgroundAmbiguityResolveRequestedFormat = PF_Unknown;
			bool bBackgroundAmbiguityResolveRequestedSRGB = false;
			FIntPoint BackgroundAmbiguityResolveRequestedExtent = FIntPoint::ZeroValue;
			FTextureRHIRef BackgroundDirectCoverageTargetRHI;
			EPixelFormat BackgroundDirectCoverageRequestedFormat = PF_Unknown;
			bool bBackgroundDirectCoverageRequestedSRGB = false;
			FIntPoint BackgroundDirectCoverageRequestedExtent = FIntPoint::ZeroValue;
			bool bBackgroundDirectCoverageTargetValid = false;
			FIntRect BackgroundDirectCoverageViewRect = FIntRect(0, 0, 0, 0);
			FTextureRHIRef AutoExposureMeterMaskTargetRHI;
			FIntPoint AutoExposureMeterMaskRequestedExtent = FIntPoint::ZeroValue;
			bool bAutoExposureMeterMaskActive = false;
		FTextureRHIRef SceneCoverageHistoryTextures[2];
		FIntPoint SceneCoverageHistoryExtent = FIntPoint::ZeroValue;
		FIntRect SceneCoverageHistoryViewRect = FIntRect(0, 0, 0, 0);
		EPixelFormat SceneCoverageHistoryFormat = PF_Unknown;
		int32 SceneCoverageHistoryReadIndex = 0;
		bool bSceneCoverageHistoryValid = false;
		TMap<UPTRINT, FPerViewRenderState> PerViewRenderStates;
		uint64 PerViewRenderStateRenderFrame = MAX_uint64;
		TMap<UPTRINT, FProxyTargetState> ProxyTargetStates;
		UPTRINT ActiveProxyTargetKey = 0u;
		uint64 ActiveProxyTargetLastUsedRenderFrame = 0u;
		uint64 ActiveProxyContentCacheGeneration = 0u;
		bool bHasActiveProxyTargetState = false;
		// Per-frame opaque tile classification (bit0=tile has opaque, bit1=tile has non-opaque),
		// built at post-opaque from pristine scene depth; consumed by the direct background draws
		// for pixel-exact splat culling. RDG resource: only valid within the frame's graph.
		float SplatPixelRadius = 1.0f;
		uint32 SHDegree = 3;
		uint32 NumSplats = 0;
		uint32 NumExpandedSplats = 0;
		uint32 NumCompressedSplats = 0;
		uint32 NumBillboardSplats = 0;
		uint32 NumInstances = 0;
		uint32 NumCullVolumes = 0;
		uint32 NumPreprocessCullVolumes = 0;
		uint32 SortCount = 0;
		uint32 PreparedSortedValueBufferIndex = 0;
		bool bPreparedForDirectDraw = false;
		uint64 PreparedDirectDrawFrameCounter = 0u;
		UPTRINT PreparedDirectDrawViewKey = 0u;
		UPTRINT PreparedDirectDrawRenderTargetKey = 0u;
		FIntRect PreparedDirectDrawViewRect = FIntRect(0, 0, 0, 0);
		FIntRect PreparedDirectDrawUnconstrainedViewRect = FIntRect(0, 0, 0, 0);
		FIntRect PreparedDirectDrawFamilyViewRect = FIntRect(0, 0, 0, 0);
		bool bEnableDirectDrawViewCaching = true;
		bool bDirectDrawViewCacheValid = false;
		FIntRect DirectDrawViewCacheViewRect = FIntRect(0, 0, 0, 0);
		FIntRect DirectDrawViewCacheUnconstrainedViewRect = FIntRect(0, 0, 0, 0);
		FIntRect DirectDrawViewCacheFamilyViewRect = FIntRect(0, 0, 0, 0);
		FMatrix44f DirectDrawViewCacheWorldToClip = FMatrix44f::Identity;
		FMatrix44f DirectDrawViewCacheWorldToView = FMatrix44f::Identity;
		uint32 DirectDrawViewCacheConfigHash = 0u;
		// Bumped every time the direct-draw view cache is rebuilt. The cache key is shared by all
		// view families, but proxy textures are per render target, so a target may only reuse its
		// textures if they were rendered for the current generation.
		uint64 DirectDrawViewCacheGeneration = 0u;
		bool bDirectDrawViewCacheVisualizeBuffer = false;
		FName DirectDrawViewCacheBufferVisualizationMode = NAME_None;
		bool bSplatTemporalSuperResolution = true;
		float SplatTemporalSuperResolutionMaxScreenPercentage = 50.0f;
		bool bBackgroundSplatsTargetValid = false;
		bool bReuseBackgroundSplatsTargetThisFrame = false;
		uint32 BackgroundSplatsTargetDrawConfigHash = 0u;
		FIntRect BackgroundProxyRasterViewRect = FIntRect(0, 0, 0, 0);
		bool bBackgroundAverageDepthTargetValid = false;
		bool bReuseBackgroundAverageDepthTargetThisFrame = false;
		uint32 BackgroundAverageDepthTargetDrawConfigHash = 0u;
		bool bBackgroundAmbiguityResolveTargetValid = false;
		bool bReuseBackgroundAmbiguityResolveTargetThisFrame = false;
		uint32 BackgroundAmbiguityResolveTargetDrawConfigHash = 0u;
		bool bAnyComponentWritesDepthToScene = true;
		// Splat raster scale for the frame being rendered, sent by the game thread with that frame.
		float FrameSplatResolutionScale = 1.0f;
		bool bAnyComponentSkipsDepthToScene = false;
		FBufferRHIRef SplatPosRadiusBufferRHI;
		FShaderResourceViewRHIRef SplatPosRadiusSRV;
		FBufferRHIRef SplatOutputIndexBufferRHI;
		FShaderResourceViewRHIRef SplatOutputIndexSRV;
		FBufferRHIRef SplatInstanceIndexBufferRHI;
		FShaderResourceViewRHIRef SplatInstanceIndexSRV;
		FBufferRHIRef SplatInstanceTransformBufferRHI;
		FShaderResourceViewRHIRef SplatInstanceTransformSRV;
		FBufferRHIRef SplatInstanceSettingsBufferRHI;
		FShaderResourceViewRHIRef SplatInstanceSettingsSRV;
		FBufferRHIRef SplatInstanceFadeSettingsBufferRHI;
		FShaderResourceViewRHIRef SplatInstanceFadeSettingsSRV;
		FBufferRHIRef CullVolumeDataBufferRHI;
		FShaderResourceViewRHIRef CullVolumeDataSRV;
		FBufferRHIRef PreprocessCullVolumeDataBufferRHI;
		FShaderResourceViewRHIRef PreprocessCullVolumeDataSRV;
		FBufferRHIRef SplatColorBufferRHI;
		FShaderResourceViewRHIRef SplatColorSRV;
		FBufferRHIRef SplatCovarianceBufferRHI;
		FShaderResourceViewRHIRef SplatCovarianceSRV;
		FBufferRHIRef SplatSHCoeffsBufferRHI;
		FShaderResourceViewRHIRef SplatSHCoeffsSRV;
		FBufferRHIRef BillboardPosRadiusBufferRHI;
		FShaderResourceViewRHIRef BillboardPosRadiusSRV;
		FBufferRHIRef BillboardAxisXBufferRHI;
		FShaderResourceViewRHIRef BillboardAxisXSRV;
		FBufferRHIRef BillboardAxisYBufferRHI;
		FShaderResourceViewRHIRef BillboardAxisYSRV;
		FBufferRHIRef BillboardOutputIndexBufferRHI;
		FShaderResourceViewRHIRef BillboardOutputIndexSRV;
		FBufferRHIRef BillboardInstanceIndexBufferRHI;
		FShaderResourceViewRHIRef BillboardInstanceIndexSRV;
		FBufferRHIRef BillboardSHCoeffsBufferRHI;
		FShaderResourceViewRHIRef BillboardSHCoeffsSRV;
		FTextureRHIRef BillboardTextureAtlasRHI;
		FBufferRHIRef CompressedPositionBufferRHI;
		FShaderResourceViewRHIRef CompressedPositionSRV;
		FBufferRHIRef CompressedOutputIndexBufferRHI;
		FShaderResourceViewRHIRef CompressedOutputIndexSRV;
		FBufferRHIRef CompressedInstanceIndexBufferRHI;
		FShaderResourceViewRHIRef CompressedInstanceIndexSRV;
		FBufferRHIRef CompressedMetadataBufferRHI;
		FShaderResourceViewRHIRef CompressedMetadataSRV;
		FBufferRHIRef CompressedCovarianceBufferRHI;
		FShaderResourceViewRHIRef CompressedCovarianceSRV;
		FBufferRHIRef CompressedSHDataBufferRHI;
		FShaderResourceViewRHIRef CompressedSHDataSRV;
		FBufferRHIRef PreprocessedSplatBufferRHI;
		FShaderResourceViewRHIRef PreprocessedSplatSRV;
		FUnorderedAccessViewRHIRef PreprocessedSplatUAV;
		FBufferRHIRef PreprocessedBillboardBufferRHI;
		FShaderResourceViewRHIRef PreprocessedBillboardSRV;
		FUnorderedAccessViewRHIRef PreprocessedBillboardUAV;
		FBufferRHIRef SortKeyBuffersRHI[2];
		FShaderResourceViewRHIRef SortKeySRVs[2];
		FUnorderedAccessViewRHIRef SortKeyUAVs[2];
		FBufferRHIRef SortValueBuffersRHI[2];
		FShaderResourceViewRHIRef SortValueSRVs[2];
		FUnorderedAccessViewRHIRef SortValueUAVs[2];
		FBufferRHIRef RadixGroupHistogramBufferRHI;
		FShaderResourceViewRHIRef RadixGroupHistogramSRV;
		FUnorderedAccessViewRHIRef RadixGroupHistogramUAV;
		FBufferRHIRef RadixPartitionStateBufferRHI;
		FShaderResourceViewRHIRef RadixPartitionStateSRV;
		FUnorderedAccessViewRHIRef RadixPartitionStateUAV;
		FBufferRHIRef RadixDigitBaseBufferRHI;
		FShaderResourceViewRHIRef RadixDigitBaseSRV;
		FUnorderedAccessViewRHIRef RadixDigitBaseUAV;
		FBufferRHIRef RadixDispatchArgsBufferRHI;
		FShaderResourceViewRHIRef RadixDispatchArgsSRV;
		FUnorderedAccessViewRHIRef RadixDispatchArgsUAV;
		FBufferRHIRef DrawIndexedArgsBufferRHI;
		FUnorderedAccessViewRHIRef DrawIndexedArgsUAV;
		// CPU sort path: preprocess counts into the scratch args so the draw keeps the CPU order count,
		// and stamps every kept splat so the draw can reject the stale slots in the CPU superset.
		FBufferRHIRef CpuSortScratchArgsBufferRHI;
		FUnorderedAccessViewRHIRef CpuSortScratchArgsUAV;
		FBufferRHIRef SplatVisibleStampBufferRHI;
		FShaderResourceViewRHIRef SplatVisibleStampSRV;
		FUnorderedAccessViewRHIRef SplatVisibleStampUAV;
		uint32 SplatVisibleStampCounter = 0u;
		uint32 PreparedVisibleStamp = 0u;
		bool bPreparedUseVisibleStampReject = false;
		TArray<FCompressedDispatchSegment> CompressedDispatchSegments;
		// Variant fade (animated variant transitions, HarmonyVariantFade.h): the slot buffers by id, and the compressed
		// segments that are drawn with the fade shader. Both empty unless a transition is running or was a moment ago.
		struct FVariantSlotBuffers
		{
			FBufferRHIRef MapBuffer;
			FShaderResourceViewRHIRef MapSRV;
			FBufferRHIRef RowsBuffer;
			FShaderResourceViewRHIRef RowsSRV;
			uint32 NumPoints = 0u;
			uint32 NumRows = 0u;
		};
		struct FVariantFadeSegment
		{
			int32 SegmentIndex = INDEX_NONE;
			FVector4f Weights = FVector4f(1.0f, 0.0f, 0.0f, 0.0f);
			uint32 SlotIds[3] = { 0u, 0u, 0u };
			uint32 SlotFlags = 0u;
			float OpacityScale = 1.0f;
		};
		TMap<uint32, FVariantSlotBuffers> VariantSlots;
		TArray<FVariantFadeSegment> VariantFadeSegments;
		FBufferRHIRef VariantDummyBuffer;
		FShaderResourceViewRHIRef VariantDummySRV;
		// External source whose buffers replace the expanded SRVs this frame (set by Harmony_UpdateState only when its
		// placeholder is the only expanded Gaussian source of the view family's world), else null.
		TSharedPtr<FHarmonyExternalSplatSource, ESPMode::ThreadSafe> ExternalExpandedSource;
	};

	FRenderThreadState::FPerViewRenderState& GetPerViewRenderState_RenderThread(const FSceneView& View);
	const FRenderThreadState::FPerViewRenderState* FindPerViewRenderState_RenderThread(const FSceneView& View) const;
	FRenderThreadState::FProxyTargetState CaptureActiveProxyTargetState_RenderThread() const;
	void ApplyActiveProxyTargetState_RenderThread(const FRenderThreadState::FProxyTargetState& State);
	void ActivateProxyTargetState_RenderThread(UPTRINT ProxyTargetKey);
	void PruneStaleProxyTargetStates_RenderThread(uint64 CurrentRenderFrame);
	void InvalidateDirectDrawState_RenderThread(bool bPreservePublishedProxy = false);
	void UpdateCpuSortScene_GameThread(
		bool bSplatDataChanged,
		bool bInstanceDataChanged,
		const TArray<FVector4f>& PosRadius,
		const TArray<FVector4f>& Color,
		const TArray<uint32>& OutputIndices,
		const TArray<uint32>& InstanceIndices,
		const TArray<FVector3f>& CompressedPosition,
		const TArray<uint32>& CompressedOutputIndices,
		const TArray<uint32>& CompressedInstanceIndices,
		const TArray<FVector4f>& BillboardPosRadius,
		const TArray<uint32>& BillboardOutputIndices,
		const TArray<uint32>& BillboardInstanceIndices,
		const TArray<FVector4f>& InstanceTransforms,
		const TArray<FVector4f>& InstanceSettings);
	// True for the one view family per frame that drives a per-frame controller; records it in InOutLastFrame.
	bool IsControllingViewFamily_GameThread(const FSceneViewFamily& InViewFamily, uint64& InOutLastFrame);
	void KickCpuSort_GameThread(const FSceneViewFamily& InViewFamily);
	// Auto sorting: picks the CPU sorter while the frame is GPU-bound, with hysteresis.
	void UpdateAutoSortDevice_GameThread();
	// Dynamic splat resolution: picks the splat raster scale for the next frame.
	void UpdateDynamicSplatResolution_GameThread(const FSceneViewFamily& InViewFamily, bool bHasSplats);
	bool IsCpuSortActive_RenderThread() const;
	FRenderThreadState RenderThreadState;
	FDelegateHandle PostOpaqueRenderDelegateHandle;
	struct FFallbackProxyRenderTargetSet
	{
		TStrongObjectPtr<UTextureRenderTarget2D> BackgroundSplats;
		TStrongObjectPtr<UTextureRenderTarget2D> BackgroundAverageDepth;
		TStrongObjectPtr<UTextureRenderTarget2D> BackgroundAmbiguityResolve;
		TStrongObjectPtr<UTextureRenderTarget2D> BackgroundDirectCoverage;
		uint64 LastUsedFrame = 0u;
	};
	TMap<UPTRINT, FFallbackProxyRenderTargetSet> FallbackProxyRenderTargetsByFamily_GameThread;
	TStrongObjectPtr<UTextureRenderTarget2D> AutoExposureMeterMaskTarget_GameThread;
	uint32 CachedSceneTopologyHash_GameThread = 0u;
	uint32 CachedSceneTransformHash_GameThread = 0u;
	// Every variant option of the (compressed) assets on screen as rows in the upload encoding, decoded once per asset
	// revision when the asset is first uploaded: a variant switch then copies rows instead of decoding the option.
	struct FVariantPatchSet
	{
		uint32 RenderRevision = 0u;
		FGuid PointSetId;
		TMap<const FHarmonySplatVariantOption*, TSharedPtr<HarmonyVariant::FCompressedPatch>> Patches;
	};
	TMap<TWeakObjectPtr<const UHarmonyDataAsset>, FVariantPatchSet> VariantPatchSets_GameThread;
	// Animated variant transitions: the per-component fade state machines (HarmonyVariantFade.h) and their slot accounting.
	struct FVariantFadeState;
	TSharedPtr<FVariantFadeState> VariantFade_GameThread;
	void ApplyVariantFade_RenderThread(FRHICommandListImmediate& RHICmdList, HarmonyVariantFade::FCommands& Commands,
		const TArray<HarmonyVariantFade::FSegment>& Segments, bool bProfile);
	TSharedPtr<FHarmonyCpuSorter, ESPMode::ThreadSafe> CpuSorter;
	TSharedPtr<FHarmonyExternalSplatSource, ESPMode::ThreadSafe> ExternalSplatSource_GameThread;
	uint64 CpuSortLastKickFrame_GameThread = MAX_uint64;
	// Last frame the main (game viewport) view family began rendering.
	uint64 LastMainViewFamilyFrame_GameThread = 0u;

	// Auto sorting state (game thread), and the choice the render thread reads.
	std::atomic<bool> bAutoSortUseCpu{ false };
	double AutoSortGpuMsAvg_GameThread = 0.0;
	double AutoSortCpuMsAvg_GameThread = 0.0;
	double AutoSortConditionSince_GameThread = -1.0;
	double AutoSortLastSwitchTime_GameThread = -1.0e9;
	double AutoSortDwellSeconds_GameThread = 0.0;

	// Dynamic splat resolution state (game thread), and the per-axis scale the render thread reads.
	std::atomic<float> SplatResolutionScale{ 1.0f };
	float DynamicResolutionMovingScale_GameThread = 1.0f;
	double DynamicResolutionGpuMsAvg_GameThread = 0.0;
	int32 DynamicResolutionStillFrames_GameThread = 0;
	int32 DynamicResolutionMovingFrames_GameThread = 0;
	int32 DynamicResolutionFramesSinceAdjust_GameThread = 0;
	double DynamicResolutionBlendStartSeconds_GameThread = -1.0;
	// Scale the controller last settled on while moving, this run only. Negative = not settled yet.
	float DynamicResolutionSettledScale_GameThread = -1.0f;
	int32 DynamicResolutionStableAdjusts_GameThread = 0;
	// Frames to skip before GPU times describe the current scale again (the GPU time lags a few frames).
	int32 DynamicResolutionWarmupFrames_GameThread = 0;
	bool bDynamicResolutionHadSplats_GameThread = false;
	uint64 DynamicResolutionLastUpdateFrame_GameThread = MAX_uint64;
	float DynamicResolutionBlendStartScale_GameThread = 1.0f;
	bool bDynamicResolutionHasLastView_GameThread = false;
	FMatrix DynamicResolutionLastViewMatrix_GameThread = FMatrix::Identity;
	FVector2D DynamicResolutionLastProjectionScale_GameThread = FVector2D::ZeroVector;
};
