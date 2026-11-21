#pragma once

#include "CoreMinimal.h"
#include "Templates/UniquePtr.h"

class AHarmonyActor;
class UHarmonyDataAsset;

// .harmony: a Harmony asset - and optionally a placed AHarmonyActor's placement and component settings - as one file that
// does not depend on the engine version: the splat buffers byte for byte in the asset's own encoding (a compressed asset
// stays compressed), the variant sets, the lighting probes' HDR images, the shadow and reflection proxies. Made to carry
// assets between engine versions (a package saved by a newer engine cannot be loaded by an older one) and readable outside
// Unreal. The layout is described in Docs/08_HarmonyFile.md and at the top of HarmonyFile.cpp.
//
// The contents are written by reflection: every saved (non-transient) property of UHarmonyDataAsset and of the Harmony
// components, so fields added later travel without format changes. A reader skips fields it does not know and keeps its
// defaults for fields the file does not have.
namespace HarmonyFile
{
    inline constexpr uint32 FormatVersion = 1;
    inline constexpr const TCHAR* Extension = TEXT("harmony");

    struct FWriteOptions
    {
        // zlib each chunk (in 8 MiB blocks, in parallel). Blocks that do not get smaller are stored as they are.
        bool bCompress = true;
    };

    // Writes Asset to Path; with Actor (whose asset it should be), also its transform, label and Harmony component settings.
    HARMONYEDITOR_API bool Write(const FString& Path, const UHarmonyDataAsset& Asset, const AHarmonyActor* Actor, FString& OutError,
        const FWriteOptions& Options = FWriteOptions());

    class HARMONYEDITOR_API FReader
    {
    public:
        FReader();
        ~FReader();

        // Reads the header, the chunk table and the manifest (not the bulk data).
        bool Open(const FString& Path, FString& OutError);

        // The name the asset had when it was written.
        FString GetAssetName() const;
        // Engine version and plugin of the writer, for logs.
        FString GetGeneratorDescription() const;

        // Fills Asset (normally a new one) from the file. The lighting probes' cubemaps are created inside Asset.
        // Warnings (fields the file has that this build does not) are appended to OutWarnings.
        bool ReadAsset(UHarmonyDataAsset& Asset, FString& OutError, TArray<FString>* OutWarnings = nullptr);

        // Whether the file was written from a placed actor.
        bool HasActor() const;
        FTransform GetActorTransform() const;
        FString GetActorLabel() const;

        // Applies the file's actor transform, label and component settings to Actor (not its asset: set that first).
        bool ApplyActor(AHarmonyActor& Actor, FString& OutError, TArray<FString>* OutWarnings = nullptr);

        struct FImpl;   // implementation detail (HarmonyFile.cpp)

    private:
        TUniquePtr<FImpl> Impl;
    };
}
