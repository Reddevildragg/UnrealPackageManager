#include "UPMPackage.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"

UUPMPackage* UUPMPackage::FromJson(TSharedPtr<FJsonObject> JsonObject)
{
    UUPMPackage* Package = NewObject<UUPMPackage>();
    Package->Name = JsonObject->GetStringField(TEXT("name"));
    Package->Version = JsonObject->GetStringField(TEXT("version"));

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

UUPMPackage* UUPMPackage::LoadOrCreatePackageJson()
{
    FString ProjectDir = FPaths::ProjectDir();
    FString UPMDir = FPaths::Combine(ProjectDir, TEXT("UPM"));
    FString PackageJsonPath = FPaths::Combine(UPMDir, TEXT("package.json"));

    if (!FPaths::DirectoryExists(UPMDir))
    {
        FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*UPMDir);
    }
    UE_LOG(LogTemp, Log, TEXT("Package.json path: %s"), *PackageJsonPath);

    if (!FPaths::FileExists(PackageJsonPath))
    {
        TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
        JsonObject->SetStringField(TEXT("name"), TEXT("UPM Plugin Package"));
        JsonObject->SetStringField(TEXT("version"), TEXT("1.0.0"));

        TSharedPtr<FJsonObject> DependenciesObject = MakeShareable(new FJsonObject);
        DependenciesObject->SetStringField(TEXT("fs-extra"), TEXT("^11.2.0"));
        DependenciesObject->SetStringField(TEXT("vue-router"), TEXT("^4.4.2"));
        JsonObject->SetObjectField(TEXT("dependencies"), DependenciesObject);

        TArray<TSharedPtr<FJsonValue>> ScopedRegistriesArray;

        auto CreateRegistry = [](const FString& Name, const FString& Url, const TArray<FString>& Scopes) -> TSharedPtr<FJsonValueObject>
        {
            TSharedPtr<FJsonObject> RegistryObject = MakeShareable(new FJsonObject);
            RegistryObject->SetStringField(TEXT("name"), Name);
            RegistryObject->SetStringField(TEXT("url"), Url);

            TArray<TSharedPtr<FJsonValue>> ScopesArray;
            for (const auto& Scope : Scopes)
            {
                ScopesArray.Add(MakeShareable(new FJsonValueString(Scope)));
            }
            RegistryObject->SetArrayField(TEXT("scopes"), ScopesArray);

            return MakeShareable(new FJsonValueObject(RegistryObject));
        };

        ScopedRegistriesArray.Add(CreateRegistry(TEXT("Atkins"), TEXT("https://npm.pkg.jetbrains.space/atkinscreativedesign/p/package-server/atkins"), { TEXT("com.atkins") }));
        ScopedRegistriesArray.Add(CreateRegistry(TEXT("Assets"), TEXT("https://npm.pkg.jetbrains.space/atkinscreativedesign/p/package-server/assets"), { TEXT("unity.assetstore"), TEXT("com.joshclose") }));
        ScopedRegistriesArray.Add(CreateRegistry(TEXT("OpenUpm"), TEXT("https://package.openupm.com"), { TEXT("com.greener-games"), TEXT("com.seb-lague"), TEXT("com.svermeulen"), TEXT("com.janniklassahn"), TEXT("com.tayx"), TEXT("com.azixmcaze"), TEXT("jillejr.newtonsoft"), TEXT("com.mbdavid.litedb"), TEXT("com.guerrillacontra"), TEXT("com.marijnzwemmer"), TEXT("com.joshclose.csvhelper"), TEXT("com.firenero"), TEXT("com.gkngkc") }));

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
            return FromJson(JsonObject);
        }
    }

    return nullptr;
}