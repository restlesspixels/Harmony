#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "HarmonyFileLibrary.generated.h"

class AHarmonyActor;
class UHarmonyDataAsset;

// .harmony export and import (HarmonyFile.h) for editor scripts (Python: unreal.HarmonyFileLibrary) and the editor menus.
UCLASS()
class HARMONYEDITOR_API UHarmonyFileLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // Writes Asset to FilePath (a .harmony file).
    UFUNCTION(BlueprintCallable, Category = "Harmony|File")
    static bool ExportHarmonyAsset(UHarmonyDataAsset* Asset, const FString& FilePath, FString& Error);

    // Writes Actor's asset, transform, label and component settings to FilePath.
    UFUNCTION(BlueprintCallable, Category = "Harmony|File")
    static bool ExportHarmonyActor(AHarmonyActor* Actor, const FString& FilePath, FString& Error);

    // Creates the Harmony asset AssetPath (a package name, such as /Game/Harmony/Scene; it must not exist yet) from FilePath.
    UFUNCTION(BlueprintCallable, Category = "Harmony|File")
    static UHarmonyDataAsset* ImportHarmonyFile(const FString& FilePath, const FString& AssetPath, bool bSave, FString& Error);

    // ImportHarmonyFile, then places an AHarmonyActor with it in the editor's level: with the file's transform, label and
    // component settings when it was exported from an actor, else at the origin with the defaults.
    UFUNCTION(BlueprintCallable, Category = "Harmony|File")
    static AHarmonyActor* ImportHarmonyFileIntoLevel(const FString& FilePath, const FString& AssetPath, bool bSave, FString& Error);
};
