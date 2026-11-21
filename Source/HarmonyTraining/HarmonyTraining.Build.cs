// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

// HarmonyTraining: RDG port of VkSplat's forward Gaussian-splat rasterizer (Phase-1 parity spike).
// Shaders live in Plugins/Harmony/Shaders/Private/Training/ and are reached through the /Plugin/Harmony
// virtual root that the Harmony module already maps (this module never maps it again).
// Must load at PostConfigInit (it contains IMPLEMENT_GLOBAL_SHADER) and after the Harmony module.
public class HarmonyTraining : ModuleRules
{
    public HarmonyTraining(ReadOnlyTargetRules Target) : base(Target)
    {
        bUsePrecompiled = true;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        // Bit-exact CPU float math in every target (CONTRACT_P3.md P9): the native init (HarmonyTrainingInit.cpp) must
        // reproduce VkSplat's float expressions exactly. Editor targets already get /fp:precise; Game/Client/Server
        // would otherwise get /fp:fast (reassociation, x/255 -> x*(1/255)). Under clang this also disables FP contraction.
        FPSemantics = FPSemanticsMode.Precise;

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "RenderCore",
                "RHI"
            });

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "Projects",
                "ImageCore",
                "ImageWrapper",
                "Json"
            });
    }
}
