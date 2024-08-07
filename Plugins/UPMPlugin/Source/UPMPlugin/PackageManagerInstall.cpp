#include "PackageManagerInstall.h"
#include "UPMPackage.h"
#include "Widgets/Input/SButton.h"
#include "Misc/MonitoredProcess.h"
#include "IPythonScriptPlugin.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

void SPackageManagerInstall::Construct(const FArguments& InArgs)
{
    ParentWindow = InArgs._ParentWindow;
    PackageData = InArgs._PackageData;

    ChildSlot
    [
        SNew(SButton)
        .Text(FText::FromString("Run Python Script"))
        .OnClicked(this, &SPackageManagerInstall::OnRunPythonScript)
    ];
}

FReply SPackageManagerInstall::OnRunPythonScript()
{
    IPythonScriptPlugin* PythonPlugin = IPythonScriptPlugin::Get();

    if (PythonPlugin)
    {
        TSharedPtr<FJsonObject> MasterJsonObject = MakeShareable(new FJsonObject);

        // Extract scoped registries from PackageData
        const TArray<FScopedRegistry>& ScopedRegistries = PackageData->ScopedRegistries;
        if (ScopedRegistries.Num() > 0)
        {
            for (const FScopedRegistry& Registry : ScopedRegistries)
            {
                TSharedPtr<FJsonObject> RegistryJsonObject = MakeShareable(new FJsonObject);
                const TArray<FString>& Scopes = Registry.Scopes;

                for (const FString& Scope : Scopes)
                {
                    FString ScriptPath = FPaths::ProjectPluginsDir() / TEXT("UPMPlugin/my_script.py");
                    FString NpmCommand = FString::Printf(TEXT("search %s --json --registry=%s"), *Scope, *Registry.Url);
                    FString Command = FString::Printf(TEXT("%s %s"), *ScriptPath, *NpmCommand);

                    if (PythonPlugin->ExecPythonCommand(*Command))
                    {
                        FString NpmOutput;
                        FString AppDataPath = FPlatformMisc::GetEnvironmentVariable(TEXT("APPDATA"));
                        FString OutputFilePath = AppDataPath / TEXT("MyApp/npm_output.txt");

                        if (FFileHelper::LoadFileToString(NpmOutput, *OutputFilePath))
                        {
                            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(NpmOutput);
                            TArray<TSharedPtr<FJsonValue>> JsonArray;
                            if (FJsonSerializer::Deserialize(Reader, JsonArray))
                            {
                                RegistryJsonObject->SetArrayField(Scope, JsonArray);
                            }
                            else
                            {
                                UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON output for scope: %s"), *Scope);
                            }
                        }
                        else
                        {
                            UE_LOG(LogTemp, Error, TEXT("Failed to read npm_output.txt for scope: %s"), *Scope);
                        }
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("Failed to execute Python script for scope: %s"), *Scope);
                    }
                }

                MasterJsonObject->SetObjectField(Registry.Name, RegistryJsonObject);
            }
        }

        // Serialize the master JSON object to a string
        FString CombinedOutput;
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&CombinedOutput);
        FJsonSerializer::Serialize(MasterJsonObject.ToSharedRef(), Writer);

        // Write the combined output to a file
        FString CombinedOutputFilePath = FPlatformMisc::GetEnvironmentVariable(TEXT("APPDATA")) / TEXT("MyApp/combined_output.json");
        if (FFileHelper::SaveStringToFile(CombinedOutput, *CombinedOutputFilePath))
        {
            UE_LOG(LogTemp, Log, TEXT("Combined output written to %s"), *CombinedOutputFilePath);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to write combined output to file"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Python Plugin not found."));
    }

    return FReply::Handled();
}