// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

// HarmonyFallbackEditor: native port of the fallback-mesh prototype (Scripts/FallbackMesh, Docs/Dev/SplatToMesh_*.md).
// Builds a simplified, textured static mesh for low-end devices from a Harmony capture dataset cache:
// GPU TSDF fusion -> marching cubes -> view importance -> weighted quadric simplification -> box unwrap packed into
// 4096^2 pages -> GPU projective bake -> (optional) glossy floor -> assets.
// Shaders: Plugins/Harmony/Shaders/Private/Fallback/ through the /Plugin/Harmony virtual root the Harmony module maps.
// Loads at PostConfigInit because it holds IMPLEMENT_GLOBAL_SHADER (as HarmonyTraining does).
public class HarmonyFallbackEditor : ModuleRules
{
    public HarmonyFallbackEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        bUsePrecompiled = true;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "Engine"
            });

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "RenderCore",
                "RHI",
                "Projects",
                "ImageCore",
                "ImageWrapper",
                "Json",
                "GeometryCore",            // FMarchingCubes tables
                "MeshDescription",
                "StaticMeshDescription",
                "AssetRegistry",
                "UnrealEd",
                "DeveloperSettings",       // UHarmonyFallbackBuildSettings
                "Slate",                   // result notification of the rig button
                "SlateCore",
                "EditorSubsystem",
                "HarmonyCapture",          // AHarmonyCaptureRig, FHarmonyCaptureEditorHooks (the rig button)
                "HarmonyTraining",         // public headers HarmonyTrainingEditor's headers include
                "MovieRenderPipelineCore",
                "MaterialEditor",          // UMaterialEditingLibrary (master materials and instances)
                "PhysicsCore",
                "Harmony",                 // UHarmonyDataAsset (soft reference to the fallback)
                "HarmonyTrainingEditor"    // dataset cache manifest reader (HarmonyTrainingDatasetCache.h)
            });
    }
}
