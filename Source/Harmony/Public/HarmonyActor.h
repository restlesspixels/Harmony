#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "HarmonyActor.generated.h"

class UHarmonyComponent;
class UHarmonyLightingComponent;
class UHarmonyShadowCatcherComponent;

UCLASS(showcategories=(Transform), hidecategories=(Replication, Collision, Physics, Networking, Input, HLOD, WorldPartition, LevelInstance, Cooking, DataLayers))
class HARMONY_API AHarmonyActor : public AActor
{
    GENERATED_BODY()

public:
    AHarmonyActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Gaussian Splat")
    TObjectPtr<UHarmonyComponent> GaussianSplatComponent;

    // Phase 13: lights the level with the asset's lighting probe (a sky light), if the asset has one.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Lighting")
    TObjectPtr<UHarmonyLightingComponent> LightingComponent;

    // Phase 14: the splats catch the shadows of the caster actors (a vehicle put into the environment).
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Lighting")
    TObjectPtr<UHarmonyShadowCatcherComponent> ShadowCatcherComponent;

    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void SetActorHiddenInGame(bool bNewHidden) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
    void SyncComponentVisibilityFromActor();
};
