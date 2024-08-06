#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UPMPackage.generated.h"

USTRUCT()
struct FScopedRegistry
{
    GENERATED_BODY()

    FString Name;
    FString Url;
    TArray<FString> Scopes;
};

UCLASS()
class UUPMPackage : public UObject
{
    GENERATED_BODY()

public:
    FString Name;
    FString Version;
    TMap<FString, FString> Dependencies;
    TArray<FScopedRegistry> ScopedRegistries;

    static UUPMPackage* FromJson(TSharedPtr<FJsonObject> JsonObject);
    static TSharedPtr<UUPMPackage> LoadOrCreatePackageJson();

    static void SavePackageJson(TSharedPtr<UUPMPackage> Package);
};