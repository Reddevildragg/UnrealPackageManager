#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "UPMPackageJson.generated.h"

// Structure to hold information about scoped registries
USTRUCT()
struct FScopedRegistry
{
    GENERATED_BODY()

    UPROPERTY()
    FString Name;

    UPROPERTY()
    FString Url;

    UPROPERTY()
    TArray<FString> Scopes;
};

// Structure to hold the package JSON data
USTRUCT()
struct FPackageJson
{
    GENERATED_BODY()

    UPROPERTY()
    TMap<FString, FString> Dependencies;

    UPROPERTY()
    TArray<FScopedRegistry> ScopedRegistries;
};

// Class to handle JSON operations for the package
UCLASS()
class UUPMPackageJson : public UObject
{
    GENERATED_BODY()

public:
    // Static paths for JSON files
    static const FString ProjectDir;
    static const FString UPMDir;
    static const FString PackageJsonPath;

    // Constructor
    UUPMPackageJson();

    // Loads the JSON file if it exists, or creates a new one with default values
    void LoadOrCreatePackageJson();

    // Saves the current data to the JSON file
    void SavePackageJson();

    // Retrieves the stored JSON object
    TSharedPtr<FPackageJson> GetJsonObject() const;

private:
    // Holds the deserialized JSON data
    TSharedPtr<FPackageJson> LoadedPackage;
};