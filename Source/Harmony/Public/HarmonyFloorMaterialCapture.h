#pragma once

#include "CoreMinimal.h"

class FRenderTarget;

// Phase 16 (notes/phase16_floor_reflections.md): a scene capture whose target is registered here gets the GBuffer's
// material in its scene colour instead of the lit scene: R roughness, G specular, B metallic, A 1 where a lit surface is
// (0: nothing, or an unlit surface). Capture with SCS_SceneColorHDR into a float target, translucency, fog and
// atmosphere off. The floor reflection bake of HarmonyTrainingEditor uses it (the engine's buffer visualisation
// materials do not give the GBuffer's values in a scene capture). Any thread.
namespace HarmonyFloorMaterialCapture
{
    HARMONY_API void RegisterTarget(const FRenderTarget* Target);
    HARMONY_API void UnregisterTarget(const FRenderTarget* Target);
    HARMONY_API bool IsTarget(const FRenderTarget* Target);
}
