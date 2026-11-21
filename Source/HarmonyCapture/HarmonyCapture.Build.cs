// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

// HarmonyCapture (CONTRACT_P4.md section 4, CPP-RIG): the Gaussian-splat capture rig AHarmonyCaptureRig.
// A self-contained port of the project's earlier spatial capture rig (grid / radial camera sample generation, ShotIndex
// decode, square 23.76 mm filmback) under Harmony names; Harmony depends on no other project plugin. It is a Runtime module because the rig is an
// actor placed in levels and evaluated in PIE / Movie Render Queue; editor-only parts (CallInEditor, property-change
// handling, metadata) are behind WITH_EDITOR. No dependency on any other project plugin.
public class HarmonyCapture : ModuleRules
{
    public HarmonyCapture(ReadOnlyTargetRules Target) : base(Target)
    {
        bUsePrecompiled = true;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        // Sample generation must give bit-identical poses to the original rig (compare_rig_poses.py). Editor targets
        // already compile /fp:precise (as the original editor module does); this keeps it explicit and stops a Game target
        // from switching to /fp:fast (reassociation would move grid points by an ulp).
        FPSemantics = FPSemanticsMode.Precise;

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
                "CinematicCamera"   // UCineCameraComponent (the rig's MainCamera)
            });

        // Phase 9: AHarmonyCaptureRig::CaptureCurrentView reads the level viewport (editor builds only).
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }
    }
}
