// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"

class AHarmonyCaptureRig;

/** What a capture rig's editor buttons ask for (handled by the HarmonyTrainingEditor module). */
enum class EHarmonyCaptureRequest : uint8
{
    CaptureOnly,       // render the rig's shots into a dataset cache
    CaptureAndTrain,   // ... then train the splat into the rig's output asset
    BakeVariants,      // base + every variant option captured, the base trained, each option baked as a delta
    RetrainFromCache,  // train the rig's output asset again from its last capture (no render)
    Cancel
};

/**
 * HarmonyCapture is a runtime module and cannot depend on the editor-only training / Movie Render Queue code, so the
 * rig's buttons go through this hook; HarmonyTrainingEditor binds it when it loads.
 */
struct HARMONYCAPTURE_API FHarmonyCaptureEditorHooks
{
    DECLARE_DELEGATE_TwoParams(FOnRequest, AHarmonyCaptureRig* /*Rig*/, EHarmonyCaptureRequest /*Request*/);
    static FOnRequest& OnRequest();

    /** The rig's "Build Fallback Mesh" button. Bound by the HarmonyFallbackEditor module (it sits on top of the training
        editor module, so the request cannot go through OnRequest's handler). */
    DECLARE_DELEGATE_OneParam(FOnBuildFallback, AHarmonyCaptureRig* /*Rig*/);
    static FOnBuildFallback& OnBuildFallback();
};
