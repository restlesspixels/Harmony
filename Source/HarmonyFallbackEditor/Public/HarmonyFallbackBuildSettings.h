// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "HarmonyFallbackSettings.h"
#include "HarmonyFallbackBuildSettings.generated.h"

/**
 * What the capture rig's "Build Fallback Mesh From Last Capture" button builds (Project Settings, Plugins, Harmony
 * Fallback Mesh). Stored per user (Saved/Config), not in the project's Config folder.
 */
UCLASS(config = EditorPerProjectUserSettings, meta = (DisplayName = "Harmony Fallback Mesh"))
class HARMONYFALLBACKEDITOR_API UHarmonyFallbackBuildSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetCategoryName() const override { return FName(TEXT("Plugins")); }

	UPROPERTY(config, EditAnywhere, Category = "Fallback Mesh", meta = (ShowOnlyInnerProperties))
	FHarmonyFallbackSettings Settings;
};
