// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

// P10 (experimental): Harmony configurable variants driven by Unreal's Variant Manager. Optional: Harmony and
// HarmonyCapture never depend on this module or on VariantManagerContent.
public class HarmonyVariantManager : ModuleRules
{
    public HarmonyVariantManager(ReadOnlyTargetRules Target) : base(Target)
    {
        bUsePrecompiled = true;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "Harmony",                 // UHarmonyComponent::SetVariant
                "HarmonyCapture",          // UHarmonyVariantSource
                "VariantManagerContent"    // ALevelVariantSetsActor, ULevelVariantSets, UVariantSet, UVariant
            });

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");   // GEditor->PlayWorld: the editor copy pauses during PIE
        }
    }
}
