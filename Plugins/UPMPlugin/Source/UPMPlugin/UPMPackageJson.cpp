#include "UPMPackageJson.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "JsonUtilities.h"

const FString UUPMPackageJson::ProjectDir = FPaths::ProjectDir();
const FString UUPMPackageJson::UPMDir = FPaths::Combine(ProjectDir, TEXT("Plugins"));
const FString UUPMPackageJson::PackageJsonPath = FPaths::Combine(UPMDir, TEXT("package.json"));

UUPMPackageJson::UUPMPackageJson()
{
    // Initialize the JSON object
    LoadedPackage = MakeShareable(new FPackageJson());
}

void UUPMPackageJson::LoadOrCreatePackageJson()
{
    // Check if the file exists
    if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*PackageJsonPath))
    {
        FString FileContent;
        if (FFileHelper::LoadFileToString(FileContent, *PackageJsonPath))
        {
            // Parse JSON content
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContent);
            TSharedPtr<FJsonObject> JsonObject;

            if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
            {
                // Deserialize the JSON object into FPackageJson
                if (JsonObject->HasTypedField<EJson::Object>(TEXT("dependencies")))
                {
                    TSharedPtr<FJsonObject> DependenciesObject = JsonObject->GetObjectField(TEXT("dependencies"));
                    for (const TPair<FString, TSharedPtr<FJsonValue>>& Item : DependenciesObject->Values)
                    {
                        LoadedPackage->Dependencies.Add(Item.Key, Item.Value->AsString());
                    }
                }

                if (JsonObject->HasTypedField<EJson::Array>(TEXT("scopedRegistries")))
                {
                    const TArray<TSharedPtr<FJsonValue>>* ArrayPtr;
                    if (JsonObject->TryGetArrayField(TEXT("scopedRegistries"), ArrayPtr))
                    {
                        for (const TSharedPtr<FJsonValue>& Value : *ArrayPtr)
                        {
                            TSharedPtr<FJsonObject> ScopedRegistryObject = Value->AsObject();
                            if (ScopedRegistryObject.IsValid())
                            {
                                FScopedRegistry ScopedRegistry;
                                ScopedRegistry.Name = ScopedRegistryObject->GetStringField(TEXT("name"));
                                ScopedRegistry.Url = ScopedRegistryObject->GetStringField(TEXT("url"));

                                const TArray<TSharedPtr<FJsonValue>>* ScopesArrayPtr;
                                if (ScopedRegistryObject->TryGetArrayField(TEXT("scopes"), ScopesArrayPtr))
                                {
                                    for (const TSharedPtr<FJsonValue>& ScopeValue : *ScopesArrayPtr)
                                    {
                                        ScopedRegistry.Scopes.Add(ScopeValue->AsString());
                                    }
                                }
                                LoadedPackage->ScopedRegistries.Add(ScopedRegistry);
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        // File does not exist, initialize with default values
        LoadedPackage->Dependencies.Add(TEXT("ExampleDependency"), TEXT("1.0.0"));
        FScopedRegistry DefaultRegistry;
        DefaultRegistry.Name = TEXT("DefaultRegistry");
        DefaultRegistry.Url = TEXT("https://default.url");
        LoadedPackage->ScopedRegistries.Add(DefaultRegistry);

        SavePackageJson(); // Save the default data
    }
}

void UUPMPackageJson::SavePackageJson()
{
    // Serialize the FPackageJson object to JSON
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

    TSharedPtr<FJsonObject> DependenciesObject = MakeShareable(new FJsonObject());
    for (const TPair<FString, FString>& Item : LoadedPackage->Dependencies)
    {
        DependenciesObject->SetStringField(Item.Key, Item.Value);
    }
    JsonObject->SetObjectField(TEXT("dependencies"), DependenciesObject);

    TArray<TSharedPtr<FJsonValue>> ScopedRegistriesArray;
    for (const FScopedRegistry& Registry : LoadedPackage->ScopedRegistries)
    {
        TSharedPtr<FJsonObject> RegistryObject = MakeShareable(new FJsonObject());
        RegistryObject->SetStringField(TEXT("name"), Registry.Name);
        RegistryObject->SetStringField(TEXT("url"), Registry.Url);

        TArray<TSharedPtr<FJsonValue>> ScopesArray;
        for (const FString& Scope : Registry.Scopes)
        {
            ScopesArray.Add(MakeShareable(new FJsonValueString(Scope)));
        }
        RegistryObject->SetArrayField(TEXT("scopes"), ScopesArray);

        ScopedRegistriesArray.Add(MakeShareable(new FJsonValueObject(RegistryObject)));
    }
    JsonObject->SetArrayField(TEXT("scopedRegistries"), ScopedRegistriesArray);

    // Serialize the JSON object to string
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
    {
        FFileHelper::SaveStringToFile(OutputString, *PackageJsonPath);
    }
}

TSharedPtr<FPackageJson> UUPMPackageJson::GetJsonObject() const
{
    return LoadedPackage;
}