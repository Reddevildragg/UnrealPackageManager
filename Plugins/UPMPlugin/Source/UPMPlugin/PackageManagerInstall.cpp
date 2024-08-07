#include "PackageManagerInstall.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/MonitoredProcess.h"

void SPackageManagerInstall::Construct(const FArguments& InArgs)
{
	FString Result;
	FString Error;
	if (RunNpmCommand(TEXT("--version"), Result, Error))
	{
		UE_LOG(LogTemp, Log, TEXT("NPM Output: %s"), *Result);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to run npm command. Error: %s"), *Error);
	}

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("This is Layout 2"))
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SButton)
			.Text(FText::FromString("Switch to Layout 1"))
			.OnClicked_Lambda([]()
			{
				// You can use the function below for changing layouts or communicate with the parent widget
				return FReply::Handled();
			})
		]
	];
}

bool SPackageManagerInstall::RunNpmCommand(const FString& Command, FString& OutResult, FString& OutError)
{
    // Define paths
    FString PluginFolder = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("UPMPlugin/Source")); // Adjust to your plugin folder path
    FString BatchFilePath = FPaths::Combine(PluginFolder, TEXT("run_npm_command.bat"));

    // Create a temp directory in the project directory
    FString TempDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Temp"));
    if (!IFileManager::Get().DirectoryExists(*TempDir))
    {
        IFileManager::Get().MakeDirectory(*TempDir);
    }

    FString OutputFilePath = FPaths::Combine(TempDir, TEXT("npm_output.txt"));
    FString ErrorFilePath = FPaths::Combine(TempDir, TEXT("npm_error.txt"));

	// Debugging: Log the file paths
	UE_LOG(LogTemp, Log, TEXT("Batch File Path: %s"), *BatchFilePath);
	UE_LOG(LogTemp, Log, TEXT("Output File Path: %s"), *OutputFilePath);
	UE_LOG(LogTemp, Log, TEXT("Error File Path: %s"), *ErrorFilePath);

    // Construct command arguments
    FString CommandArgs = FString::Printf(TEXT("\"%s\" \"%s\" %s"), *OutputFilePath, *ErrorFilePath, *Command);

	UE_LOG(LogTemp, Log, TEXT("Error File Path: %s"), *CommandArgs);

    // Execute the batch file
    FProcHandle ProcessHandle = FPlatformProcess::CreateProc(
        *BatchFilePath,
        *CommandArgs,
        true,
        false,
        false,
        nullptr,
        0,
        nullptr,
        nullptr,
        nullptr
    );

    if (!ProcessHandle.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to execute batch file."));
        return false;
    }

    // Wait for the batch file process to complete
    FPlatformProcess::WaitForProc(ProcessHandle);

    // Read the output and error files
    if (!FFileHelper::LoadFileToString(OutResult, *OutputFilePath))
    {
        OutResult = TEXT("Failed to read output file.");
    }
    if (!FFileHelper::LoadFileToString(OutError, *ErrorFilePath))
    {
        OutError = TEXT("Failed to read error file.");
    }

    // Get the return code
    int32 ReturnCode;
    FPlatformProcess::GetProcReturnCode(ProcessHandle, &ReturnCode);

    // Cleanup
    FPlatformProcess::CloseProc(ProcessHandle);
    //IFileManager::Get().Delete(*OutputFilePath);
    //IFileManager::Get().Delete(*ErrorFilePath);

    return (ReturnCode == 0);
}