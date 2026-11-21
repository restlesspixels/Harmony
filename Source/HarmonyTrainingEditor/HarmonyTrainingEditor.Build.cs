// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

// HarmonyTrainingEditor (CONTRACT_P4.md P1): editor glue between HarmonyTraining (Runtime) and the Harmony asset.
// - Converts trained splats (FHarmonyTrainingSplatsCPU) to FHarmonyRawData with the PLY importer's own code
//   (FHarmonySplatOps, exported from HarmonyEditor) and writes / overwrites a UHarmonyDataAsset package, compressed with
//   Harmony::Editor::CompressDataAssetInPlace (exported from HarmonyEditor).
// - Registers HarmonyTraining::GetAssetWriterHook() (TrainSplat option "out_asset") in StartupModule.
// - UHarmonyTrainingEditorLibrary: Python entry points for the headless tests (Scripts/EnvGen/splat_asset_test.py).
// - CONTRACT_P4.md P3: UHarmonyTrainingEditorSubsystem drives a non-blocking FHarmonyTrainingSession per editor tick
//   (FTickableEditorObject), shows it live through Harmony's renderer (zero-copy FHarmonyExternalSplatSource) and saves
//   it with the asset writer; Python API for Scripts/EnvGen/splat_train_in_editor.py.
// - CONTRACT_P4.md P4: UHarmonyTrainingMoviePipelineOutput (an MRQ output, engine plugin MovieRenderPipeline, declared in
//   Harmony.uplugin) builds the training dataset + depth-seeded init in memory and hands them to the subsystem.
public class HarmonyTrainingEditor : ModuleRules
{
    public HarmonyTrainingEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        bUsePrecompiled = true;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        // The converter reproduces the PLY round trip (HarmonyTraining WritePly -> Harmony PLY importer) bit for bit, so its
        // few float expressions (logit, sigmoid) must not be reassociated. Editor targets already compile /fp:precise;
        // this only makes it explicit (same setting as HarmonyTraining.Build.cs).
        FPSemantics = FPSemanticsMode.Precise;

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "EditorSubsystem",  // UEditorSubsystem (HarmonyTrainingEditorSubsystem.h)
                "Harmony",          // UHarmonyDataAsset, FHarmonyRawData, AHarmonyActor, UHarmonySubsystem (public headers)
                "HarmonyTraining",  // FHarmonyTrainingSplatsCPU (public header), LoadPly / WritePly / UnreorderSH, GetAssetWriterHook
                "MovieRenderPipelineCore",  // P4: UMoviePipelineOutputBase (public base of UHarmonyTrainingMoviePipelineOutput)
                "HarmonyCapture"            // AHarmonyCaptureRig / FHarmonyCaptureJobSettings (HarmonyCaptureJobSubsystem.h)
            });

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "AssetRegistry",       // FAssetRegistryModule::AssetCreated
                "CinematicCamera",     // P4: UCineCameraComponent (CurrentFocalLength, Filmback) of the MRQ camera
                "LevelSequence",       // one-click capture: the in-memory ULevelSequence (HarmonyCaptureJobSubsystem)
                "MovieRenderPipelineEditor",        // ... rendered with UMoviePipelinePIEExecutor
                "MovieRenderPipelineRenderPasses",  // ... UMoviePipelineDeferredPassBase
                "MovieSceneTracks",    // ... ShotIndex float track, camera cut
                "Slate",               // ... progress notification
                "SlateCore",
                "ImageCore",           // P4: FImage, FImageCore::CopyImage (FinalImage -> BGRA8 sRGB)
                "ImageWrapper",        // P7: mask PNGs (IImageWrapper, G8)
                "ImageWriteQueue",     // P4: FImagePixelData (ImagePixelData.h) of the MRQ FinalImage pass
                "Json",                // P6: project settings option merge, dataset cache manifest
                "MovieScene",          // P4: MovieSceneHelpers::CameraComponentFromRuntimeObject (MovieSceneCommonHelpers.h)
                "DeveloperSettings",   // UHarmonyEditorImportSettings derives from UDeveloperSettings
                "HarmonyEditor",       // FHarmonySplatOps, CompressDataAssetInPlace, UHarmonyEditorImportSettings
                "RenderCore",          // ENQUEUE_RENDER_COMMAND, GGameThreadTime (RenderTimer.h)
                "RHI",                 // FShaderResourceViewRHIRef (HarmonyExternalSplatSource.h, HarmonyTrainingSession.h)
                "UnrealEd",            // SavePackages, FTickableEditorObject, FEditorDelegates, GEditor
                "ToolMenus",           // "Export to PLY..." on Harmony assets (HarmonyExportPlyMenu.cpp)
                "ContentBrowser",      // ... UContentBrowserAssetContextMenuContext
                "DesktopPlatform"      // ... folder picker
            });
    }
}
