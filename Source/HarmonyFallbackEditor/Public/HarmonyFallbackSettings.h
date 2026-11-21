// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HarmonyFallbackSettings.generated.h"

/** One output budget of the fallback builder: a triangle count and a number of atlas pages. */
USTRUCT(BlueprintType)
struct HARMONYFALLBACKEDITOR_API FHarmonyFallbackTier
{
	GENERATED_BODY()

	/** Asset name suffix ("Low" -> SM_<name>_Fallback_Low). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fallback")
	FString Name = TEXT("Mid");

	/** Triangle budget of the fused shell (the sky dome adds 1,280). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fallback", meta = (ClampMin = "1000"))
	int32 Triangles = 300000;

	/** Atlas pages (PageSize x PageSize each). One page uses the single-atlas density recipe (cap 64 texels/m at half
	    the capture density), two or more the cap-128 recipe (SplatToMesh_M1_Results.md section 6). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fallback", meta = (ClampMin = "1", ClampMax = "16"))
	int32 Pages = 2;

	FHarmonyFallbackTier() = default;
	FHarmonyFallbackTier(const TCHAR* InName, int32 InTriangles, int32 InPages) : Name(InName), Triangles(InTriangles), Pages(InPages) {}
};

/** How BuildFallbackOptions builds an option (SplatToMesh_M4_Variants.md). */
UENUM(BlueprintType)
enum class EHarmonyFallbackVariantGeometry : uint8
{
	/** Compare the option's depth with the base capture's: share the base meshes when nothing moved, else build a mesh of its own. */
	Auto,
	/** Always share the base meshes (re-bake the appearance only), whatever the detection says. */
	ShareBaseGeometry,
	/** Always build the option as a complete fallback of its own. */
	OwnMesh
};

/** One variant option to build: the names it has in the splat asset's variant sets and its capture (same poses as the base). */
USTRUCT(BlueprintType)
struct HARMONYFALLBACKEDITOR_API FHarmonyFallbackOptionSource
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fallback")
	FName VariantSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fallback")
	FName Option;

	/** The option's dataset cache (a folder, or a cache name under the project dataset cache root). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fallback")
	FString Cache;
};

/**
 * Settings of UHarmonyFallbackLibrary::BuildFallbackFromCache. Defaults are the M1 recipe
 * (Docs/Dev/SplatToMesh_M1_Results.md section 8) plus the box-projected cubemap floor
 * (SplatToMesh_GlossyFloor_Results.md). Lengths are metres in the capture frame unless stated.
 */
USTRUCT(BlueprintType)
struct HARMONYFALLBACKEDITOR_API FHarmonyFallbackSettings
{
	GENERATED_BODY()

	FHarmonyFallbackSettings();

	// ---- outputs ------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	TArray<FHarmonyFallbackTier> Tiers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output", meta = (ClampMin = "256", ClampMax = "8192"))
	int32 PageSize = 4096;

	/** Asset base name; empty = taken from the capture map of the cache ("Env_Showroom" -> "Showroom"). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	FString AssetName;

	/** When set, every stage also writes its result here (raw mesh, importance, simplified meshes, UV meshes as .npy,
	    atlas pages as .png, timings as build.json) so each stage can be compared with the Python prototype. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	FString IntermediateDir;

	/** After writing the assets, record them on the splat asset the capture trained (UHarmonyDataAsset::FallbackTiers,
	    soft references by tier name; options by variant set and option) and save it. The rig button sets this. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bLinkToSplatAsset = false;

	/** With bLinkToSplatAsset: the UHarmonyDataAsset to record on instead of the one the cache names (a long package
	    name or object path; empty = the capture's own). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	FString LinkAssetPath;

	/** Write Unreal assets (static mesh, textures, materials). Off: only the intermediate files. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output")
	bool bWriteAssets = true;

	/** Stop after this stage: "" (all), "load", "fuse", "importance", "simplify", "unwrap", "bake". */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output", AdvancedDisplay)
	FString StopAfter;

	/** Debug: read the raw fusion (mesh_raw_V.npy / mesh_raw_F.npy) from this folder instead of fusing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output", AdvancedDisplay)
	FString ReuseRawMeshDir;

	/** Debug: read the simplified meshes (mesh_<tier>_V.npy / _F.npy) from this folder instead of simplifying. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output", AdvancedDisplay)
	FString ReuseSimplifiedDir;

	/** Use every n-th training view (1 = all). For quick tests only. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output", AdvancedDisplay, meta = (ClampMin = "1"))
	int32 ViewStep = 1;

	// ---- fusion -------------------------------------------------------------------------------------------------
	/** Voxel size of the innermost level. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fusion", meta = (ClampMin = "0.005"))
	float Voxel = 0.03f;

	/** Level 0 covers the camera bounding box expanded by this much; every further level doubles voxel and margin. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fusion", meta = (ClampMin = "0.5"))
	float Margin = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fusion", meta = (ClampMin = "1", ClampMax = "8"))
	int32 Levels = 5;

	/** Truncation band and the band kept behind the surface, in voxels. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fusion")
	float TruncVoxels = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fusion")
	float BackVoxels = 2.0f;

	/** Minimum-view-count lock: voxels seen as a surface by at least this many views resist carving (0 = off). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fusion")
	int32 LockViews = 3;

	/** Weight factor of free-space votes in locked voxels. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fusion")
	float LockWeight = 0.05f;

	/** Depth-edge pixels use the projective distance instead of their captured-normal plane (thinner outlines). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fusion")
	bool bEdgeProjective = true;

	/** Relative depth step above which a pixel is a depth edge. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fusion", AdvancedDisplay)
	float EdgeRel = 0.04f;

	/** Connected components with fewer triangles than this are dropped from the raw fusion. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fusion")
	int32 MinComponentTriangles = 200;

	/** Canopy hull closing in image space before fusion (M1 section 7). Off by default: its foliage detector is depth-edge
	    density and misfires on hard-surface scenes; it needs a class signal in the capture. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fusion|Foliage")
	bool bFoliageHullClosing = false;

	/** Window (pixels, odd) in which foliage pixels take the nearest depth. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fusion|Foliage", meta = (ClampMin = "3", ClampMax = "31"))
	int32 FoliageDilate = 9;

	/** Share of depth-edge pixels in the 7x7 window above which a pixel counts as foliage. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fusion|Foliage")
	float FoliageDensity = 0.3f;

	/** Apply the hull closing from this nested level outwards only. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fusion|Foliage")
	int32 FoliageMinLevel = 0;

	// ---- importance and simplification --------------------------------------------------------------------------
	/** Distance up to which geometry has full weight; beyond it the weight falls as (Z0 / z)^Power. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simplify")
	float ImportanceZ0 = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simplify")
	float ImportancePower = 2.0f;

	/** Weight of vertices no training view sees. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simplify")
	float UnseenWeight = 0.02f;

	/** Boost for occlusion-edge and thin-structure vertices (times the share of views in which they are one). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simplify")
	float SilhouetteBoost = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simplify")
	float ThinBoost = 30.0f;

	/** Weight of the perpendicular quadrics on open boundary edges. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simplify", AdvancedDisplay)
	float BoundaryWeight = 1.0f;

	/** A surviving face may not rotate by more than acos(this) in a collapse. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simplify", AdvancedDisplay)
	float MinNormalDot = 0.5f;

	// ---- unwrap -------------------------------------------------------------------------------------------------
	/** Charts are also cut on a grid of this size. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unwrap")
	float ChartCell = 4.0f;

	/** Texel density cap (texels per metre) for one page / for two or more pages. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unwrap")
	float MaxDensityOnePage = 64.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unwrap")
	float MaxDensityMultiPage = 128.0f;

	/** Scale on the capture density (pixels per metre at the nearest view) for one page / several pages. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unwrap")
	float DensityScaleOnePage = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unwrap")
	float DensityScaleMultiPage = 1.0f;

	/** Empty texels kept around every chart; chart origins are aligned to GutterAlign texels so that mips up to
	    log2(GutterAlign) never mix two charts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unwrap", meta = (ClampMin = "1", ClampMax = "8"))
	int32 Gutter = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unwrap", meta = (ClampMin = "1", ClampMax = "16"))
	int32 GutterAlign = 4;

	// ---- bake ---------------------------------------------------------------------------------------------------
	/** Depth test of a texel against the captured depth: |d - z| < Abs + Rel * z. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bake")
	float BakeTolAbs = 0.03f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bake")
	float BakeTolRel = 0.01f;

	/** Inward-facing dome textured from the sky pixels (radius in metres; 0 = no dome). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bake")
	float SkyDomeRadius = 3000.0f;

	// ---- glossy floor -------------------------------------------------------------------------------------------
	/** Box-projected cubemap reflection on the polished floor (if the capture has one). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glossy Floor")
	bool bGlossyFloor = true;

	/** Reflection weight T = Gain * (F0 + (1 - F0) * (1 - cos)^Power). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glossy Floor")
	float FloorGain = 0.8735f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glossy Floor")
	float FloorF0 = 0.02933f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glossy Floor")
	float FloorFresnelPower = 4.1111f;

	/** Blur cone of the reflection (radians). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glossy Floor")
	float FloorBlurCone = 0.07434f;

	/** Cubemap face size and its height above the floor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glossy Floor")
	int32 FloorCubeSize = 512;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glossy Floor")
	float FloorCubeHeight = 1.5f;

	/** Cubemap position in the capture frame (metres); zero = above the middle of the polished floor at FloorCubeHeight. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glossy Floor", AdvancedDisplay)
	FVector FloorCubePosition = FVector::ZeroVector;

	/** Proxy box of the reflection in the capture frame (metres, up is -Y); both zero = the outline of the polished
	    floor up to the ceiling found from the mesh. The prototype fits this box per scene (it matters: the glazing a
	    floor reflects is usually well outside the floor's outline). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glossy Floor", AdvancedDisplay)
	FVector FloorBoxMin = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glossy Floor", AdvancedDisplay)
	FVector FloorBoxMax = FVector::ZeroVector;

	// ---- variant options (BuildFallbackOptions) -----------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variants")
	EHarmonyFallbackVariantGeometry VariantGeometry = EHarmonyFallbackVariantGeometry::Auto;

	/** A pixel's depth changed when |option - base| > Abs + Rel * min(option, base), or when only one of the two is valid. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variants")
	float VariantDepthTolAbs = 0.02f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variants")
	float VariantDepthTolRel = 0.01f;

	/** Changed pixels are eroded by this many pixels per view first (drops one-pixel flicker along depth edges). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variants", meta = (ClampMin = "0", ClampMax = "4"))
	int32 VariantErodePx = 1;

	/** Size of the voxels the changed surface is recorded in (metres). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variants", meta = (ClampMin = "0.02"))
	float VariantVoxel = 0.25f;

	/** A voxel counts as changed when at least this many changed pixels from at least this many views fall into it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variants", meta = (ClampMin = "1"))
	int32 VariantMinVoxelPixels = 16;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variants", meta = (ClampMin = "1"))
	int32 VariantMinVoxelViews = 2;

	/** ... and those views are at least this share of all views that see the voxel's surface (changed or not). A glitch in
	    a few frames of one capture is outvoted by the views that see the same place unchanged. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variants", meta = (ClampMin = "0", ClampMax = "1"))
	float VariantMinVoxelViewShare = 0.25f;

	/** Geometry changed when at least this many voxels gained or lost surface. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variants", meta = (ClampMin = "1"))
	int32 VariantMinChangedVoxels = 1;

	/** Compare every n-th view (1 = all). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variants", AdvancedDisplay, meta = (ClampMin = "1"))
	int32 VariantDetectViewStep = 1;
};
