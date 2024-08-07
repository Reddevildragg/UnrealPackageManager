#include "PackageManagerInstall.h"
#include "Widgets/Input/SButton.h"
#include "Misc/MonitoredProcess.h"
#include "IPythonScriptPlugin.h"

void SPackageManagerInstall::Construct(const FArguments& InArgs)
{
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
		TArray<FString> Scopes = { TEXT("@greener-games"), TEXT("@greener-games"), TEXT("@greener-games") };
		TArray<TSharedPtr<FJsonValue>> CombinedJsonArray;

		for (const FString& Scope : Scopes)
        {
            FString ScriptPath = FPaths::ProjectPluginsDir() / TEXT("UPMPlugin/my_script.py");
            FString NpmCommand = FString::Printf(TEXT("search %s --json"), *Scope);
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
                        CombinedJsonArray.Append(JsonArray);
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

        // Serialize the combined JSON array to a string
        FString CombinedOutput;
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&CombinedOutput);
        FJsonSerializer::Serialize(CombinedJsonArray, Writer);

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

FString ReadPythonScriptOutput()
{
	FString Result;
	FFileHelper::LoadFileToString(Result, TEXT("result.txt"));
	return Result;
}