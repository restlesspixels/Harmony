// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Math/Matrix.h"
#include "RenderGraphFwd.h"
#include "RenderGraphResources.h"   // FRDGPooledBuffer (complete type for TRefCountPtr members)
#include "RHIGPUReadback.h"          // FRHIGPUBufferReadback (complete type for TSharedPtr members)
#include "RHIShaderPlatform.h"
#include "Templates/Function.h"     // TFunction (GetAssetWriterHook)

class FRHICommandListImmediate;

/**
 * CPU-side splat data in exactly the layout the GPU kernels consume (VkSplat device layout).
 * See H:/SplatTools/spike/CONTRACT.md and CONTRACT_P2.md.
 */
struct HARMONYTRAINING_API FHarmonyTrainingSplatsCPU
{
	int32 Num = 0;
	TArray<float> Xyz;              // 3N, tightly packed, COLMAP frame, metres
	TArray<FVector4f> SHCoeffs;     // 12*ceil32(N), SH_REORDER_SIZE=32 reordered, zero padded
	TArray<FVector4f> Rotations;    // N, (w,x,y,z) raw PLY rot_0..3
	TArray<FVector4f> ScalesOpacs;  // N, (exp(scale_0..2), sigmoid(opacity))
};

/** One COLMAP camera/image pair, already downscaled. */
struct HARMONYTRAINING_API FHarmonyTrainingCamera
{
	FString ImageName;
	int32 Width = 0, Height = 0;
	float Fx = 0, Fy = 0, Cx = 0, Cy = 0;
	FMatrix44f WorldToCamera = FMatrix44f::Identity;  // M[r][c] = W2C(r,c), COLMAP R|t (translation in M[0..2][3]), metres
};

namespace HarmonyTraining
{
	/** SH_REORDER_SIZE of VkSplat (config.slang:40). */
	constexpr int32 SHReorderSize = 32;
	/** float4s per splat in the SH buffer (48 floats). */
	constexpr int32 SHFloat4PerSplat = 12;
	/** Tile size of the rasterizer. */
	constexpr int32 TileSize = 16;
	/** Upper clamp of the intersection capacity (4 sort buffers of Capacity uints = 1 GiB at this value).
	    HTSetupIndirect.usf clamps the TileRanges group count to 65535 and HTTileRanges.usf grid-strides, so M may
	    exceed 65535*256. */
	constexpr int32 MaxIntersectionCapacity = 1 << 26;
	/** Largest splat count the current usf files handle: ProjectForwardCS indexes with SV_DispatchThreadID.x only, so
	    its ceil(N/32) groups must not wrap (65535 groups per dimension). Raising it needs GetUnWrappedDispatchThreadId
	    (ComputeShaderUtils.ush) in ProjectForward / GenerateKeys / Scan. The Phase 2 fused projection backward + Adam
	    (32 threads, ceil(N/32) groups, not wrapped) has the same bound. */
	constexpr int32 MaxSplats = 65535 * 32;

	/** Number of float4 elements of the reordered SH buffer (and of each SH Adam moment buffer) for N splats. */
	inline int32 NumSHFloat4(int32 NumSplats)
	{
		return SHFloat4PerSplat * Align(FMath::Max(NumSplats, 0), SHReorderSize);
	}

	/** Binary little-endian PLY (3DGS layout, float properties found by name, any order). */
	HARMONYTRAINING_API bool LoadPly(const FString& Path, FHarmonyTrainingSplatsCPU& Out, FString& OutError);

	/** cameras.txt + images.txt (PINHOLE / SIMPLE_PINHOLE); sorted by image name; Downscale (>=1) divides W,H,fx,fy,cx,cy. */
	HARMONYTRAINING_API bool LoadColmapText(const FString& SparseDir, int32 Downscale, TArray<FHarmonyTrainingCamera>& Out, FString& OutError);

	/** COLMAP qvec (w,x,y,z, normalised here) + tvec -> world-to-camera FMatrix44f with M[r][c] = W2C(r,c), computed in
	    double and cast to float per element. Exactly the matrix LoadColmapText builds (CONTRACT_P4 P0). */
	HARMONYTRAINING_API FMatrix44f MakeWorldToCameraColmap(double Qw, double Qx, double Qy, double Qz, double Tx, double Ty, double Tz);

	/** The 4 splat input buffers registered in a graph. */
	struct FRDGSplatBuffers
	{
		FRDGBufferRef Xyz = nullptr;          // StructuredBuffer<float>  3N
		FRDGBufferRef SHCoeffs = nullptr;     // StructuredBuffer<float4> 12*ceil32(N)
		FRDGBufferRef Rotations = nullptr;    // StructuredBuffer<float4> N
		FRDGBufferRef ScalesOpacs = nullptr;  // StructuredBuffer<float4> N
		int32 NumSplats = 0;
	};

	/** Outputs of one forward render. Every buffer is transient in the graph that produced it. */
	struct FRDGForwardOutputs
	{
		FRDGBufferRef PixelState = nullptr;       // StructuredBuffer<float4> W*H: (C.rgb premultiplied over black, T), row-major
		FRDGBufferRef Stats = nullptr;            // StructuredBuffer<uint> 4: [0]=total intersections, [1]=M after clamp, [2]=overflow
		FRDGBufferRef NContributors = nullptr;    // StructuredBuffer<int> W*H
		uint32 KeyMask = 0;
		uint32 TileBits = 0;
		uint32 DepthBits = 0;

		// Phase 2: the forward's per-splat buffers and the sort result, for the backward passes.
		FRDGBufferRef XyVs = nullptr;             // StructuredBuffer<float2> N
		FRDGBufferRef InvCovVsOpacity = nullptr;  // StructuredBuffer<float4> N
		FRDGBufferRef Rgb = nullptr;              // StructuredBuffer<float>  3N
		FRDGBufferRef TilesTouched = nullptr;     // StructuredBuffer<int>    N
		FRDGBufferRef SortedValues = nullptr;     // Buffer<uint> (PF_R32_UINT), Capacity: splat index or 0xFFFFFFFF
		FRDGBufferRef TileRanges = nullptr;       // StructuredBuffer<int>    T+1
		uint32 Width = 0, Height = 0, GridWidth = 0, GridHeight = 0, NumSplats = 0;
	};

	/**
	 * Uploads the CPU splat arrays into RDG structured buffers. The upload does NOT copy: Splats must stay alive and
	 * unchanged until GraphBuilder.Execute() has returned.
	 */
	HARMONYTRAINING_API FRDGSplatBuffers CreateSplatBuffers(FRDGBuilder& GraphBuilder, const FHarmonyTrainingSplatsCPU& Splats);

	/**
	 * Adds VkSplat's forward rasterizer for one camera: ProjectForward -> inclusive scan -> SetupIndirect -> GenerateKeys
	 * -> GPU radix sort -> TileRanges -> RasterizeForward. All intermediate buffers are transient per call.
	 * IntersectionCapacity is clamped to [1, MaxIntersectionCapacity]. ActiveSH (SH degree, clamped to 0..3) is
	 * HT_ActiveSH. bAntialiased: Mip-Splatting's 2D filter (VkSplat ANTIALIASE_MODE 1; TrainSplat option antialiased).
	 * Render thread only.
	 */
	HARMONYTRAINING_API FRDGForwardOutputs AddForwardRasterPasses(
		FRDGBuilder& GraphBuilder,
		const FRDGSplatBuffers& Splats,
		const FHarmonyTrainingCamera& Camera,
		int32 IntersectionCapacity,
		int32 ActiveSH = 3,
		bool bAntialiased = false);

	/** Sort key layout (VkSplat formula): tileBits = ceil(log2(T)), depthBits = min(uint(31.99 - log2(T)), 23). */
	HARMONYTRAINING_API uint32 ComputeSortKeyMask(uint32 NumTiles, uint32& OutTileBits, uint32& OutDepthBits);

	/**
	 * True when every HarmonyTraining global shader is compiled for the given platform; lists the missing ones otherwise.
	 * The Phase 2 (training) shaders compile for SM6 PC platforms only (CONTRACT_P2.md D3); bIncludeTraining = false
	 * checks the Phase 1 forward shaders only (for forward-only callers on an SM5 RHI).
	 */
	HARMONYTRAINING_API bool AreShadersAvailable(EShaderPlatform Platform, FString& OutMissing, bool bIncludeTraining = true);

	// ================================================================================================================
	// Phase 2 (CONTRACT_P2.md section 4). All RDG functions: render thread only.
	// ================================================================================================================

	// ---- loss ----
	struct FRDGLossOutputs
	{
		FRDGBufferRef VPixelState = nullptr;  // StructuredBuffer<float4> W*H: (dL/dC.rgb, 0)
		FRDGBufferRef SsimMap = nullptr;      // StructuredBuffer<float4> 3*W*H: [pix*3+c] = (ssim, dS/dmu1, dS/dE[x^2], dS/dE[xy])
	};

	/** SsimForward + SsimBackward. RefImage: StructuredBuffer<uint> of W*H RGBA8 (R in the low byte). */
	FRDGLossOutputs AddLossPasses(FRDGBuilder& GraphBuilder, FRDGBufferRef PixelState, FRDGBufferRef RefImage, uint32 W, uint32 H, float SsimLambda,
		FRDGBufferRef BgImage = nullptr);   // Phase 7b: W*H RGBA8 background (render = C + T * B); null = none

	/** LossPartial + LossFinal; writes LossOut[Slot] = (loss, l1_mean, ssim_mean, mse) (float4 structured buffer owned by the caller). */
	void AddLossReducePasses(FRDGBuilder& GraphBuilder, FRDGBufferRef PixelState, FRDGBufferRef RefImage, FRDGBufferRef SsimMap,
		uint32 W, uint32 H, float SsimLambda, FRDGBufferRef LossOut, uint32 Slot, FRDGBufferRef BgImage = nullptr);

	// ---- raster backward ----
	struct FRDGSplatGradients
	{
		FRDGBufferRef VXyVs = nullptr;             // structured stride 4, 2N floats (written as uint via CAS)
		FRDGBufferRef VInvCovVsOpacity = nullptr;  // structured stride 4, 4N floats (conic xyz, opacity w)
		FRDGBufferRef VRgb = nullptr;              // structured stride 4, 3N floats
	};

	/** Clears and fills the per-splat gradients from the forward's buffers and VPixelState (PerSplat backward, CAS adds). */
	FRDGSplatGradients AddRasterizeBackwardPasses(FRDGBuilder& GraphBuilder, const FRDGForwardOutputs& Fwd, FRDGBufferRef VPixelState);

	/** Phase 8: RasterizeForwardCS again over InOut's sorted lists / alphas with InOut.Rgb (any per-splat 3-vector); writes
	    new InOut.PixelState and InOut.NContributors. */
	void AddRasterizeForwardOnly(FRDGBuilder& GraphBuilder, const FHarmonyTrainingCamera& Camera, FRDGForwardOutputs& InOut);

	/**
	 * Phase 8 depth supervision (HarmonyTrainingDepth.cpp): renders expected depth over Fwd's lists, L = Lambda / P *
	 * sum w |D - (1 - T) gt| / gt (GtDepth: ceil(P/2) uint, two fp16 metres per uint, 0 = none; w = RefImage alpha), adds
	 * its alpha / screen gradients into InOutGrads and returns the 3N world-space mean gradient for the Adam pass.
	 * OutLossPartials (optional): ceil(P/256) float2 (sum |e|/gt * w, sum w).
	 */
	FRDGBufferRef AddDepthSupervisionPasses(FRDGBuilder& GraphBuilder, const FRDGSplatBuffers& Params, const FRDGForwardOutputs& Fwd,
		const FHarmonyTrainingCamera& Camera, FRDGBufferRef GtDepth, FRDGBufferRef RefImage, float Lambda, FRDGSplatGradients& InOutGrads,
		FRDGBufferRef* OutLossPartials = nullptr, float FrontMargin = -1.0f);   // FrontMargin >= 0: depth_mode "front"

	// ---- fused projection backward + Adam ----
	struct FRDGAdamBuffers
	{
		FRDGBufferRef Xyz = nullptr;   // float 6N: float 3*(2g+k), k=0 m.xyz, k=1 v.xyz
		FRDGBufferRef SH1 = nullptr;   // float4 12*ceil32(N), m, SH reorder layout
		FRDGBufferRef SH2 = nullptr;   // float4 12*ceil32(N), v, SH reorder layout
		FRDGBufferRef Rot = nullptr;   // float4 2N: [2g] m, [2g+1] v
		FRDGBufferRef ScOp = nullptr;  // float4 2N: [2g] m, [2g+1] v (u-space: log scale, logit opacity)
	};

	struct FHarmonyTrainingOptimizerSettings
	{
		int32 Step = 0;          // python step; the kernel gets Step+1 (HT_AdamStep)
		int32 ActiveSH = 3;      // SH degree 0..3
		float LrMeans = 0;       // already scheduled and scene-scaled (lr(t=Step+1))
		float LrQuats = 0.001f, LrScales = 0.005f, LrOpacities = 0.05f, LrShDc = 0.0025f, LrShRest = 0.0025f / 20.0f;
		float RegScale = 0, RegOpacity = 0;   // already divided by 3N and N
		// Anisotropy regulariser (TrainSplat aniso_lambda / aniso_ratio / aniso_mode): loss += AnisoScale *
		// sum_g max(s_max / s_b, AnisoRatio) - AnisoRatio, s_b = s_mid (AnisoMode 0) or s_min (1). AnisoScale is
		// aniso_lambda / N; 0 = off (the kernel then runs exactly the unregularised path).
		float AnisoScale = 0, AnisoRatio = 4.0f;
		uint32 AnisoMode = 0;
		bool bAntialiased = false;   // TrainSplat option antialiased: the backward of ANTIALIASE_MODE 1 (match the forward's)
	};

	/** One dense Adam step for every splat: ProjectionBackwardAdamCS, 32 threads, ceil(N/32) groups. Updates Params and Adam in place. */
	/** ExtraMeanGrad (Phase 8, optional): 3N floats added to the world-space mean gradient before Adam. */
	void AddProjectionBackwardAdamPass(FRDGBuilder& GraphBuilder, const FRDGSplatBuffers& Params, const FRDGAdamBuffers& Adam,
		FRDGBufferRef TilesTouched, const FRDGSplatGradients& Grads,
		const FHarmonyTrainingCamera& Camera, const FHarmonyTrainingOptimizerSettings& Settings, FRDGBufferRef ExtraMeanGrad = nullptr);

	// ---- persistent GPU state (pooled buffers; render thread only) ----
	struct FHarmonyTrainingGPUState
	{
		/** Current (logical) splat count: what RegisterSplats reports and ReadbackGPUState reads. MCMC callers update it
		    on the render thread as the CPU schedule grows the set. */
		int32 NumSplats = 0;
		/** Allocated rows of all 9 buffers (CONTRACT_P3 P6): cap_max for MCMC, NumSplats otherwise. NumSplats <= Capacity. */
		int32 Capacity = 0;
		TRefCountPtr<FRDGPooledBuffer> Xyz, SH, Rot, ScOp, AdamXyz, AdamSH1, AdamSH2, AdamRot, AdamScOp;
	};

	/**
	 * Allocates all 9 buffers at Capacity rows (0 = Splats.Num; reusing Out's buffers when the sizes match), uploads the
	 * Splats.Num rows, zeroes Adam over the full capacity. When Capacity > Splats.Num the parameter rows beyond N are
	 * zeroed first (an extra graph). Capacity must be in [Splats.Num, MaxSplats]. Graphs are executed here. The upload
	 * does not copy: the caller must SubmitAndBlockUntilGPUIdle or flush before freeing or changing Splats.
	 */
	HARMONYTRAINING_API void InitGPUState(FRHICommandListImmediate& RHICmdList, const FHarmonyTrainingSplatsCPU& Splats, FHarmonyTrainingGPUState& Out, int32 Capacity = 0);
	HARMONYTRAINING_API FRDGSplatBuffers RegisterSplats(FRDGBuilder& GraphBuilder, const FHarmonyTrainingGPUState& State);
	FRDGAdamBuffers RegisterAdam(FRDGBuilder& GraphBuilder, const FHarmonyTrainingGPUState& State);
	void AddZeroAdamPasses(FRDGBuilder& GraphBuilder, const FRDGAdamBuffers& Adam);

	/** Adam moments in device layout (SH1/SH2 reordered, 12*ceil32(N) each). */
	struct FHarmonyTrainingAdamCPU
	{
		TArray<float> Xyz;                                   // 6N
		TArray<FVector4f> SH1, SH2, Rot, ScOp;               // 12*ceil32(N), 12*ceil32(N), 2N, 2N
	};

	/** Blocking (SubmitAndBlockUntilGPUIdle inside): reads params (and Adam if OutAdam) back into device-layout CPU arrays. */
	HARMONYTRAINING_API void ReadbackGPUState(FRHICommandListImmediate& RHICmdList, const FHarmonyTrainingGPUState& State, FHarmonyTrainingSplatsCPU& OutSplats, FHarmonyTrainingAdamCPU* OutAdam);

	// ---- generic readback helper (inside a graph; resolve after Execute + SubmitAndBlockUntilGPUIdle) ----
	struct FHTReadback
	{
		TSharedPtr<FRHIGPUBufferReadback> Readback;
		uint32 NumBytes = 0;
		TArray<uint8>* Dest = nullptr;
	};

	/** Enqueues a copy of the first NumBytes of Buffer; Dest is filled by ResolveReadbacks. Dest must outlive the resolve. */
	void AddReadback(FRDGBuilder& GraphBuilder, FRDGBufferRef Buffer, uint32 NumBytes, TArray<uint8>& Dest, TArray<FHTReadback>& InOut);

	/** Lock/copy/Unlock each entry into its Dest (Dest is emptied when a lock fails), then empties InOut. Render thread, after the GPU is idle. */
	void ResolveReadbacks(TArray<FHTReadback>& InOut);

	/**
	 * Inclusive int32 scan of Input[0..Count) into Output (HTScan.usf, groupshared only, no wave ops). Both buffers are
	 * StructuredBuffer<int> with at least Count elements. The forward rasterizer's scan, exported for MCMC (Phase 3).
	 */
	void AddInclusiveScanPasses(FRDGBuilder& GraphBuilder, FRDGBufferRef Input, FRDGBufferRef Output, uint32 Count);

	// ================================================================================================================
	// Phase 3: MCMC densification (CONTRACT_P3.md section 4; HTMCMC.usf; HarmonyTrainingMCMC.cpp). Render thread only.
	// ================================================================================================================

	/** One step's MCMC post-backward work. Every count is CPU-known (no readback, CONTRACT_P3 P5). */
	struct FHarmonyTrainingMCMCStep
	{
		uint32 NumSplats = 0;     // N of this step (pre-add): relocation and noise run over [0, N), add writes [N, N + NumAdd)
		uint32 NumAdd = 0;        // 0 = no add
		uint32 Seed = 0;          // MCMCStepSeed(mcmc_seed, step); relocation (Seed, tid), add (Seed+1, tid), noise (tid, Seed ^ 0x6E6F6973)
		bool bRefine = false;     // relocate dead splats
		bool bNoise = true;       // inject noise over [0, N)
		float MinOpacity = 0.005f;
		float MaxScale = 0.f;     // > 0: max(scale) > MaxScale counts as dead (robust option, 0 = VkSplat)
		float NoiseScale = 0.f;   // noise_lr * LrMeansVk(step), 0-based step
		float NoiseMaxDisp = 0.f; // > 0: clamps |kick| (robust option, 0 = VkSplat)
		// 16 Nov 2026, error-guided relocation (TrainSplat reloc_score / reloc_jitter): RelocMode 0 = opacity (VkSplat),
		// 1 = error, 2 = opacity x error; ErrAccum (float2 per row, HTErrAccum.usf) is required when RelocMode != 0.
		uint32 RelocMode = 0;
		float RelocJitter = 0.f;
		FRDGBufferRef ErrAccum = nullptr;
	};

	/** RDG buffers of the last relocate / add, for RunMCMCStep (null when that part did not run). */
	struct FHTMCMCDebug
	{
		FRDGBufferRef RelocIndexMap = nullptr;   // StructuredBuffer<uint> N
		FRDGBufferRef RelocCounts = nullptr;     // StructuredBuffer<uint> N
		FRDGBufferRef AddIndexMap = nullptr;     // StructuredBuffer<uint> NumAdd
		FRDGBufferRef AddCounts = nullptr;       // StructuredBuffer<uint> N
	};

	/**
	 * Appends relocate (if bRefine), add (if NumAdd > 0) and noise (if bNoise) for one step, in that order (VkSplat
	 * executeMCMCPostBackward without Morton). Params/Adam must be allocated for at least NumSplats + NumAdd rows; the
	 * caller sets the logical N of the following steps to NumSplats + NumAdd. Params.NumSplats is ignored (Step.NumSplats
	 * wins). MCMCStats: StructuredBuffer<uint>(4), caller-cleared: [0] += dead (relocated), [1] += NumAdd, [2] += skipped
	 * noise kicks, [3] |= error bits (1 = rand_max == 0). OutDebug (optional): receives the index maps / counts.
	 */
	void AddMCMCPostBackwardPasses(FRDGBuilder& GraphBuilder, const FRDGSplatBuffers& Params, const FRDGAdamBuffers& Adam,
		const FHarmonyTrainingMCMCStep& Step, FRDGBufferRef MCMCStats, FHTMCMCDebug* OutDebug = nullptr);

	/** Phase 7 culling: radius (metres from the origin, 0 = off) and axis-aligned boxes (COLMAP metres). */
	struct FHarmonyTrainingCull
	{
		double Radius = 0.0;
		TArray<FBox3f> Boxes;
		float CulledOpacity = 1.0e-6f;   // below the forward's 1/255 cull and the MCMC min_opacity
		bool IsActive() const { return Radius > 0.0 || Boxes.Num() > 0; }
		/** CPU twin of HTCull.usf's test. */
		bool IsCulled(const FVector3f& P) const;
	};

	/** HTCull.usf CullCS over NumSplats rows of Params (opacity := min(o, CulledOpacity) for culled centres). */
	void AddCullPass(FRDGBuilder& GraphBuilder, const FRDGSplatBuffers& Params, uint32 NumSplats, const FHarmonyTrainingCull& Cull);

	/** CONTRACT_P3 P4: (uint32)SplitMix64Next(S), S = Seed ^ 0x6D636D635F726E67 ^ Step * 0x9E3779B97F4A7C15 (uint64).
	    Test vectors: (0,0) 0x54e593a4, (0,600) 0x96a76320, (0,29999) 0x8446fb2b, (1,600) 0xe19f0f3f. */
	HARMONYTRAINING_API uint32 MCMCStepSeed(uint64 Seed, int32 Step);

	// ---- CPU data helpers (HarmonyTrainingData.cpp) ----
	/** CONTRACT_P2.md section 5 state dir (meta.json, xyz/rot/scale/opac/sh .f32) -> device layout (SH reordered). */
	HARMONYTRAINING_API bool LoadStateDir(const FString& Dir, FHarmonyTrainingSplatsCPU& Out, FString& OutError);
	/** Device layout -> state dir (SH un-reordered to (N,16,3)). Creates Dir. */
	HARMONYTRAINING_API bool WriteStateDir(const FString& Dir, const FHarmonyTrainingSplatsCPU& Splats, FString& OutError);
	/** Directory -> LoadStateDir, file -> LoadPly. */
	HARMONYTRAINING_API bool LoadSplats(const FString& PathOrDir, FHarmonyTrainingSplatsCPU& Out, FString& OutError);
	/** VkSplat writePLY layout (gs_trainer.cpp:1398-1462): log scale, logit opacity (no clamp), raw quats, f_rest channel-major. */
	HARMONYTRAINING_API bool WritePly(const FString& Path, const FHarmonyTrainingSplatsCPU& Splats, FString& OutError);
	/** (N,16,3) [k][c] floats -> device layout, 12*ceil32(N) float4, zero padded. */
	HARMONYTRAINING_API void ReorderSH(const float* ShNx16x3, int32 N, TArray<FVector4f>& Out);
	/** Device layout -> (N,16,3) [k][c] floats (48N). */
	HARMONYTRAINING_API void UnreorderSH(const TArray<FVector4f>& Dev, int32 N, TArray<float>& OutNx16x3);
	/** VkSplat scene_scale (gs_trainer.cpp:460-476): 1.1 * max distance of all camera centres from their mean, in double. */
	HARMONYTRAINING_API double ComputeSceneScale(const TArray<FHarmonyTrainingCamera>& AllCameras);

	// ---- Phase 4: asset writer hook (CONTRACT_P4.md section 2, P0 option out_asset) ----
	/**
	 * Writer for TrainSplat / TrainSplatFromDataset option "out_asset" (a long package name). HarmonyTraining is a Runtime
	 * module and must not depend on editor modules, so the editor module (HarmonyTrainingEditor) registers the writer here
	 * in StartupModule (GetAssetWriterHook() = ...) and clears it in ShutdownModule (GetAssetWriterHook() = nullptr).
	 * Called on the game thread, after the PLY (and the state dir) are written and before eval:
	 *   Hook(LongPackageName, FinalSplats, bAntialiased, OutError) -> true on success (bAntialiased: option antialiased,
	 *   the asset is marked to be drawn with the same compensation); false fails the run with OutError.
	 * FinalSplats is the final readback in device layout (FHarmonyTrainingSplatsCPU: SH reordered, ScalesOpacs activated);
	 * it is only valid for the duration of the call. With out_asset set and no hook registered the run fails before
	 * training with "ERROR: out_asset needs HarmonyTrainingEditor".
	 */
	using FHarmonyTrainingAssetWriter = TFunction<bool(const FString& /*LongPackageName*/, const FHarmonyTrainingSplatsCPU& /*Splats*/, bool /*bAntialiased*/, FString& /*OutError*/)>;
	HARMONYTRAINING_API TFunction<bool(const FString&, const FHarmonyTrainingSplatsCPU&, bool, FString&)>& GetAssetWriterHook();
}
