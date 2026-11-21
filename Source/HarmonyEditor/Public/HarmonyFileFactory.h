#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"

#include "HarmonyFileFactory.generated.h"

// Imports a .harmony file (HarmonyFile.h) as a Harmony asset: drag it into the Content Browser or use Import. Actor settings
// in the file are not used here: Tools > Harmony > Import .harmony into Level places the actor with them.
UCLASS()
class HARMONYEDITOR_API UHarmonyFileFactory : public UFactory
{
    GENERATED_BODY()

public:
    UHarmonyFileFactory();

    virtual bool FactoryCanImport(const FString& Filename) override;

    virtual UObject* FactoryCreateFile(
        UClass* InClass,
        UObject* InParent,
        FName InName,
        EObjectFlags Flags,
        const FString& Filename,
        const TCHAR* Parms,
        FFeedbackContext* Warn,
        bool& bOutOperationCanceled) override;
};
