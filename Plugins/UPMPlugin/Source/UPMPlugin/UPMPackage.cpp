#include "UPMPackage.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"

const FString UUPMPackage::ProjectDir = FPaths::ProjectDir();
const FString UUPMPackage::UPMDir = FPaths::Combine(ProjectDir, TEXT("Plugins"));
const FString UUPMPackage::PackageJsonPath = FPaths::Combine(UPMDir, TEXT("package.json"));

UUPMPackage* UUPMPackage::FromJson(TSharedPtr<FJsonObject> JsonObject)
{
    UUPMPackage* Package = NewObject<UUPMPackage>();

    const TSharedPtr<FJsonObject>* DependenciesObject;
    if (JsonObject->TryGetObjectField(TEXT("dependencies"), DependenciesObject))
    {
        for (const auto& Elem : (*DependenciesObject)->Values)
        {
            Package->Dependencies.Add(Elem.Key, Elem.Value->AsString());
        }
    }

    const TArray<TSharedPtr<FJsonValue>>* ScopedRegistriesArray;
    if (JsonObject->TryGetArrayField(TEXT("scopedRegistries"), ScopedRegistriesArray))
    {
        for (const auto& Elem : *ScopedRegistriesArray)
        {
            TSharedPtr<FJsonObject> RegistryObject = Elem->AsObject();
            FScopedRegistry Registry;
            Registry.Name = RegistryObject->GetStringField(TEXT("name"));
            Registry.Url = RegistryObject->GetStringField(TEXT("url"));

            const TArray<TSharedPtr<FJsonValue>>* ScopesArray;
            if (RegistryObject->TryGetArrayField(TEXT("scopes"), ScopesArray))
            {
                for (const auto& ScopeElem : *ScopesArray)
                {
                    Registry.Scopes.Add(ScopeElem->AsString());
                }
            }

            Package->ScopedRegistries.Add(Registry);
        }
    }

    return Package;
}

// Use static constants in the methods
TSharedPtr<UUPMPackage> UUPMPackage::LoadOrCreatePackageJson()
{
    if (!FPaths::DirectoryExists(UPMDir))
    {
        FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*UPMDir);
    }
    UE_LOG(LogTemp, Log, TEXT("Package.json path: %s"), *PackageJsonPath);

    if (!FPaths::FileExists(PackageJsonPath))
    {
        TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
        TSharedPtr<FJsonObject> DependenciesObject = MakeShareable(new FJsonObject);
        JsonObject->SetObjectField(TEXT("dependencies"), DependenciesObject);

        TArray<TSharedPtr<FJsonValue>> ScopedRegistriesArray;

        JsonObject->SetArrayField(TEXT("scopedRegistries"), ScopedRegistriesArray);

        FString OutputString;
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
        if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
        {
            FFileHelper::SaveStringToFile(OutputString, *PackageJsonPath);
        }
    }

    FString JsonString;
    if (FFileHelper::LoadFileToString(JsonString, *PackageJsonPath))
    {
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

        if (FJsonSerializer::Deserialize(Reader, JsonObject))
        {
            UUPMPackage* Package = FromJson(JsonObject);
            return MakeShareable(Package);
        }
    }

    return nullptr;
}

void UUPMPackage::SavePackageJson(TSharedPtr<UUPMPackage> Package)
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

    TSharedPtr<FJsonObject> DependenciesObject = MakeShareable(new FJsonObject);
    for (const auto& Elem : Package->Dependencies)
    {
        DependenciesObject->SetStringField(Elem.Key, Elem.Value);
    }
    JsonObject->SetObjectField(TEXT("dependencies"), DependenciesObject);

    TArray<TSharedPtr<FJsonValue>> ScopedRegistriesArray;
    for (const auto& Registry : Package->ScopedRegistries)
    {
        TSharedPtr<FJsonObject> RegistryObject = MakeShareable(new FJsonObject);
        RegistryObject->SetStringField(TEXT("name"), Registry.Name);
        RegistryObject->SetStringField(TEXT("url"), Registry.Url);

        TArray<TSharedPtr<FJsonValue>> ScopesArray;
        for (const auto& Scope : Registry.Scopes)
        {
            ScopesArray.Add(MakeShareable(new FJsonValueString(Scope)));
        }
        RegistryObject->SetArrayField(TEXT("scopes"), ScopesArray);

        ScopedRegistriesArray.Add(MakeShareable(new FJsonValueObject(RegistryObject)));
    }
    JsonObject->SetArrayField(TEXT("scopedRegistries"), ScopedRegistriesArray);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
    {
        FFileHelper::SaveStringToFile(OutputString, *PackageJsonPath);
    }
}
