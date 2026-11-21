// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HarmonyTrainingTypes.h"

/**
 * Phase 3 native init from COLMAP points3D (CONTRACT_P3.md P9, notes/phase3_init.md). CPU only, any thread.
 * Reproduces VkSplat's load_colmap_dataset init (gs_trainer.cpp:155-556) bit for bit for xyz, scale, opacity and SH;
 * rotations come from our own seeded counter-based generator (VkSplat's mt19937 + std::normal_distribution stream is
 * not reproducible outside its binary). Numpy reference: H:/SplatTools/spike/parity/init_ref.py.
 */
namespace HarmonyTraining
{
	/**
	 * Reads <SparseDir>/points3D.bin, or <SparseDir>/points3D.txt when there is no .bin (VkSplat's preference).
	 * Output order is ascending POINT3D_ID; for a duplicate ID the LAST record in the file wins (std::map semantics of
	 * colmap_reader.cpp). xyz is parsed as double and cast to float (round to nearest); rgb .txt values are int, wrapped
	 * modulo 256 as VkSplat's (uint8_t)int. OutRgb.A = 255. Malformed lines / truncated records are errors (VkSplat: UB).
	 */
	HARMONYTRAINING_API bool LoadColmapPoints(const FString& SparseDir, TArray<FVector3f>& OutXyz, TArray<FColor>& OutRgb, FString& OutError);

	/**
	 * Builds the init state in device layout (SH reordered, zero padded to ceil32(N)):
	 *   xyz = point; scale = InitScale * dist_avg (isotropic, activated); opacity = InitOpacity (activated);
	 *   SH DC = (float(rgb) / 255.0f - 0.5f) / 0.28209479177387814f, SH rest = 0;
	 *   rotation = InitRandomUnitQuat(RotSeed, i) (w,x,y,z).
	 * dist_avg = sqrtf((d0^2 + .. + d(M-1)^2) / float(M-1)) over the M = min(N,4) nearest points including the point
	 * itself, d_k = sqrtf(s_k) with the glm float metric s = (dx*dx + dy*dy) + dz*dz (exact kd-tree, ComputeKnnDistAvg).
	 * Errors: N < 2 (VkSplat divides 0/0 at N = 1), Xyz/Rgb size mismatch, non-finite coordinates, N too large.
	 */
	HARMONYTRAINING_API bool BuildInitFromPoints(const TArray<FVector3f>& Xyz, const TArray<FColor>& Rgb, float InitScale, float InitOpacity,
	                                             uint64 RotSeed, FHarmonyTrainingSplatsCPU& Out, FString& OutError);

	/** Exact k = 4 nearest-neighbour dist_avg (VkSplat formula above) for every point; Points must be finite, Num >= 2.
	    Multithreaded (ParallelFor) query; the result does not depend on thread count or tie order. */
	HARMONYTRAINING_API void ComputeKnnDistAvg(const TArray<FVector3f>& Points, TArray<float>& OutDistAvg);

	/** Phase 8: per point, the PCA of its 16 nearest neighbours (self included): normal = smallest-variance axis, tangent =
	    largest, planarity = lambda_min / lambda_mid (0 = perfectly planar; 1 when undetermined). */
	HARMONYTRAINING_API void ComputeKnnNormals(const TArray<FVector3f>& Points, TArray<FVector3f>& OutNormal, TArray<FVector3f>& OutTangent,
		TArray<float>& OutPlanarity);

	/** Phase 8 (init_orient "normals"): splats whose neighbourhood is planar (planarity <= MaxPlanarity) become discs on that
	    plane: rotation = (tangent, normal x tangent, normal), scale.z = scale.x * Flatten. Returns the number oriented. */
	HARMONYTRAINING_API int32 OrientInitToNormals(FHarmonyTrainingSplatsCPU& Splats, float Flatten, float MaxPlanarity);

	/** SplitMix64 output function (Mix(Seed + (Key+1)*0x9E3779B97F4A7C15) = the (Key+1)-th SplitMix64(Seed) output).
	    Test vector: InitMix64(0x9E3779B97F4A7C15) = 0xE220A8397B1DCDAF. */
	HARMONYTRAINING_API uint64 InitMix64(uint64 Z);

	/** Uniform unit quaternion (w,x,y,z) for splat Index: rejection sample in the 4-ball from 24-bit SplitMix64 uniforms,
	    then normalised with 1/sqrtf. Bit-exact with init_ref.py init_rotations (phase3_init.md section 4). */
	HARMONYTRAINING_API FVector4f InitRandomUnitQuat(uint64 Seed, uint32 Index);
}
