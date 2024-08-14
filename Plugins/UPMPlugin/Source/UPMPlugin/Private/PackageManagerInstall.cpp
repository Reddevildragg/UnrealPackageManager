#include "PackageManagerInstall.h"
#include "UPMPackageJson.h"
#include "Widgets/Input/SButton.h"
#include "IPythonScriptPlugin.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UPMPlugin/Public/UPMWindow.h"

const FString PackageJsonPath = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("package.json"));
const FString AppDataPath = FPlatformMisc::GetEnvironmentVariable(TEXT("APPDATA"));

const FString AppDataId = "UnrealPackageManager";
const FString ProjectName = FApp::GetProjectName(); // or use a unique ID if available

const FString OutputFolder = AppDataPath / AppDataId / ProjectName;
const FString OutputFilePath = OutputFolder / TEXT("npm_output.txt");
const FString CombinedOutputFilePath = OutputFolder / TEXT("combined_output.json");

const FString ProcessScopesPath = FPaths::ProjectPluginsDir() / TEXT("UPMPlugin/my_script.py");
const FString InstallNpmPath = FPaths::ProjectPluginsDir() / TEXT("UPMPlugin/install_package.py");


void SPackageManagerInstall::Construct(const FArguments& InArgs)
{
	ParentWindow = InArgs._ParentWindow;
	PackageJsonHandlerPtr = InArgs._PackageJsonHandler;
	if (PackageJsonHandlerPtr == nullptr)
	{
		// Display an error message if the package is not valid
		ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().Padding(10).FillHeight(1)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Failed to load or create package.json")))
				.ColorAndOpacity(FLinearColor::Red)
			]
		];
		return;
	}


	SAssignNew(RegistrySelectBox, SVerticalBox)
	+ SVerticalBox::Slot()
	.Padding(15)
	.AutoHeight()
	[
		SNew(STextBlock)
		.Text(FText::FromString(TEXT("Select a registry from the left to view details.")))
	];

	// Load the combined output file if available
	LoadCombinedOutput();

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(0, 10, 0, 0)
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Install Packages:"))
		]
		+ SVerticalBox::Slot()
		.Padding(0, 10, 0, 0)
		.FillHeight(1.0f)
		[
			SNew(SSplitter)
			+ SSplitter::Slot()
			.Value(0.2f)
			[
				RegistrySelectBox.ToSharedRef()
			]
			+ SSplitter::Slot()
			.Value(0.2f)
			[
				SAssignNew(ScopeSelectBox, SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Select a registry from the left to view details.")))
				]
			]
			+ SSplitter::Slot()
			.Value(0.2f)
			[
				SAssignNew(AssetSelectBox, SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Select a registry from the left to view details.")))
				]
			]
			+ SSplitter::Slot()
			.Value(0.4f)
			[
				SAssignNew(AssetInformationBox, SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Select a registry from the left to view details.")))
				]
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SButton)
			.Text(FText::FromString("Refresh Scopes"))
			.OnClicked(this, &SPackageManagerInstall::FetchPackageInformation)
		]
	];
}

void SPackageManagerInstall::OnDeselect()
{
	ResetAssetSelection();
	ResetScopeSelection();

	SelectedRegistryName = "";
	SelectedScopeName = "";
	SelectedAssetName = "";

	SelectedAsset = nullptr;
}

void SPackageManagerInstall::OnSelect()
{
	FDateTime PackageJsonTime = IFileManager::Get().GetTimeStamp(*PackageJsonPath);
	FDateTime CombinedOutputTime = IFileManager::Get().GetTimeStamp(*CombinedOutputFilePath);

	if (PackageJsonTime > CombinedOutputTime)
	{
		FetchPackageInformation();
	}


}

FReply SPackageManagerInstall::FetchPackageInformation()
{
	IPythonScriptPlugin* PythonPlugin = IPythonScriptPlugin::Get();

	if (PythonPlugin)
	{
		TSharedPtr<FJsonObject> MasterJsonObject = MakeShareable(new FJsonObject);

		// Extract scoped registries from PackageData
		const TArray<FScopedRegistry>& ScopedRegistries = PackageJsonHandlerPtr->GetJsonObject()->ScopedRegistries;
		if (ScopedRegistries.Num() > 0)
		{
			for (const FScopedRegistry& Registry : ScopedRegistries)
			{
				TSharedPtr<FJsonObject> RegistryJsonObject = MakeShareable(new FJsonObject);
				const TArray<FString>& Scopes = Registry.Scopes;

				// Save the name and URL of the registry inside a meta tag
				TSharedPtr<FJsonObject> MetaJsonObject = MakeShareable(new FJsonObject);
				MetaJsonObject->SetStringField(TEXT("name"), Registry.Name);
				MetaJsonObject->SetStringField(TEXT("url"), Registry.Url);
				RegistryJsonObject->SetObjectField(TEXT("meta"), MetaJsonObject);

				// Save all scope information inside a scopes tag
				TSharedPtr<FJsonObject> ScopesJsonObject = MakeShareable(new FJsonObject);
				for (const FString& Scope : Scopes)
				{
					FString NpmCommand = FString::Printf(TEXT("search %s --json --registry=%s"), *Scope, *Registry.Url);
					FString Command = FString::Printf(TEXT("%s %s %s"), *ProcessScopesPath, *OutputFilePath,
					                                  *NpmCommand);
					UE_LOG(LogTemp, Log, TEXT("Package.json path: %s"), *Command);
					if (PythonPlugin->ExecPythonCommand(*Command))
					{
						FString NpmOutput;

						if (FFileHelper::LoadFileToString(NpmOutput, *OutputFilePath))
						{
							TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(NpmOutput);
							TArray<TSharedPtr<FJsonValue>> JsonArray;
							if (FJsonSerializer::Deserialize(Reader, JsonArray))
							{
								ScopesJsonObject->SetArrayField(Scope, JsonArray);
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

				RegistryJsonObject->SetObjectField(TEXT("scopes"), ScopesJsonObject);
				MasterJsonObject->SetObjectField(Registry.Name, RegistryJsonObject);
			}
		}

		// Serialize the master JSON object to a string
		FString CombinedOutput;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&CombinedOutput);
		FJsonSerializer::Serialize(MasterJsonObject.ToSharedRef(), Writer);

		// Write the combined output to a file
		if (FFileHelper::SaveStringToFile(CombinedOutput, *CombinedOutputFilePath))
		{
			UE_LOG(LogTemp, Log, TEXT("Combined output written to %s"), *CombinedOutputFilePath);

			// Load the combined output file to create buttons
			LoadCombinedOutput();
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

void SPackageManagerInstall::LoadCombinedOutput()
{
	FString CombinedOutput;
	if (FFileHelper::LoadFileToString(CombinedOutput, *CombinedOutputFilePath))
	{
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(CombinedOutput);
		if (FJsonSerializer::Deserialize(Reader, LoadedData) && LoadedData.IsValid())
		{
			GenerateRegistryButtons();
		}
	}
}

void SPackageManagerInstall::GenerateRegistryButtons()
{
	if (RegistrySelectBox.IsValid())
	{
		RegistrySelectBox->ClearChildren();

		for (const auto& RegistryEntry : LoadedData->Values)
		{
			TSharedPtr<FJsonObject> RegistryObject = RegistryEntry.Value->AsObject();
			TSharedPtr<FJsonObject> MetaObject = RegistryObject->GetObjectField(TEXT("meta"));
			FString RegistryName = MetaObject->GetStringField(TEXT("name"));
			bool bHasValidScope = false;

			// Access the scopes tag within the RegistryObject
			TSharedPtr<FJsonObject> ScopesObject = RegistryObject->GetObjectField(TEXT("scopes"));
			for (const auto& ScopeEntry : ScopesObject->Values)
			{
				const TArray<TSharedPtr<FJsonValue>>* AssetsArray;
				if (ScopesObject->TryGetArrayField(ScopeEntry.Key, AssetsArray) && AssetsArray->Num() > 0)
				{
					bHasValidScope = true;
					break;
				}
			}

			if (bHasValidScope)
			{
				RegistrySelectBox->AddSlot().AutoHeight()
				[
					SNew(SButton)
					             .Text(FText::FromString(*RegistryName))
					             .ButtonColorAndOpacity(this, &SPackageManagerInstall::GetRegistryButtonColor,
					                                    RegistryName) // Apply highlight effect
					             .OnClicked_Lambda([this, RegistryName]()
					             {
						             return OnRegistryButtonClicked(RegistryName);
					             })
				];
			}
		}
	}
}

FReply SPackageManagerInstall::OnRegistryButtonClicked(FString RegistryName)
{
	SelectedRegistryName = RegistryName; // Set the selected registry name
	ResetScopeSelection();

	// Clear the ScopeSelectBox before adding new buttons
	if (ScopeSelectBox.IsValid())
	{
		ScopeSelectBox->ClearChildren();

		// Find the selected registry in the loaded data
		if (LoadedData.IsValid())
		{
			SelectedRegistryObject = LoadedData->GetObjectField(RegistryName);
			if (SelectedRegistryObject.IsValid())
			{
				// Access the scopes tag within the SelectedRegistryObject
				TSharedPtr<FJsonObject> ScopesObject = SelectedRegistryObject->GetObjectField(TEXT("scopes"));
				for (const auto& ScopeEntry : ScopesObject->Values)
				{
					const FString& Scope = ScopeEntry.Key;
					const TArray<TSharedPtr<FJsonValue>>* AssetsArray;

					// Check if the scope has a non-empty array of assets
					if (ScopesObject->TryGetArrayField(Scope, AssetsArray) && AssetsArray->Num() > 0)
					{
						ScopeSelectBox->AddSlot().AutoHeight()
						[
							SNew(SButton)
							             .Text(FText::FromString(Scope))
							             .ButtonColorAndOpacity(this, &SPackageManagerInstall::GetScopeButtonColor,
							                                    Scope) // Apply highlight effect
							             .OnClicked_Lambda([this, Scope]()
							             {
								             return OnScopeButtonClicked(Scope);
							             })
						];
					}
				}
			}
		}
	}

	return FReply::Handled();
}

FReply SPackageManagerInstall::OnScopeButtonClicked(FString ScopeName)
{
	SelectedScopeName = ScopeName; // Set the selected scope name
	ResetAssetSelection();
	// Clear the AssetSelectBox before adding new buttons
	if (AssetSelectBox.IsValid())
	{
		AssetSelectBox->ClearChildren();
		AssetMap.Empty();

		// Find the selected scope in the loaded data
		if (SelectedRegistryObject.IsValid())
		{
			TSharedPtr<FJsonObject> ScopesObject = SelectedRegistryObject->GetObjectField(TEXT("scopes"));
			SelectedScopeObject = ScopesObject->GetArrayField(ScopeName);
			for (const TSharedPtr<FJsonValue>& AssetValue : SelectedScopeObject)
			{
				TSharedPtr<FJsonObject> AssetObject = AssetValue->AsObject();
				if (AssetObject.IsValid())
				{
					FString AssetName = AssetObject->GetStringField("name");
					AssetMap.Add(AssetName, AssetObject); // Populate the asset map

					AssetSelectBox->AddSlot().AutoHeight()
					[
						SNew(SButton)
						             .Text(FText::FromString(AssetName))
						             .ButtonColorAndOpacity(this, &SPackageManagerInstall::GetAssetButtonColor,
						                                    AssetName) // Apply highlight effect
						             .OnClicked_Lambda([this, AssetName]()
						             {
							             return OnAssetButtonClicked(AssetName);
						             })
					];
				}
			}
		}
	}

	return FReply::Handled();
}

FReply SPackageManagerInstall::OnAssetButtonClicked(FString AssetName)
{
	SelectedAssetName = AssetName; // Set the selected asset name

	// Clear the AssetInformationBox before adding new information
	if (AssetInformationBox.IsValid())
	{
		AssetInformationBox->ClearChildren();

		// Find the selected asset in the asset map
		if (AssetMap.Contains(AssetName))
		{
			TSharedPtr<FJsonObject> AssetObject = AssetMap[AssetName];
			if (AssetObject.IsValid())
			{
				SelectedAsset = AssetObject;

				// Display asset information

				if (AssetObject->HasField("name"))
				{
					FString Description = AssetObject->GetStringField("name");
					AssetInformationBox->AddSlot().AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString("Name: " + Description))
					];
				}

				if (AssetObject->HasField("description"))
				{
					FString Description = AssetObject->GetStringField("description");
					AssetInformationBox->AddSlot().AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString("Description: " + Description))
					];
				}

				if (AssetObject->HasField("version"))
				{
					FString Version = AssetObject->GetStringField("version");
					AssetInformationBox->AddSlot().AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString("Version: " + Version))
					];
				}

				if (AssetObject->HasField("date"))
				{
					FString Date = AssetObject->GetStringField("date");
					AssetInformationBox->AddSlot().AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString("Date: " + Date))
					];
				}

				FString PackageName = AssetObject->GetStringField("name");
				FString PluginsDir = FPaths::ProjectPluginsDir();
				FString NodeModulesPath = FPaths::Combine(PluginsDir, TEXT("node_modules"), PackageName);
				FString PluginPath = FPaths::Combine(PluginsDir, PackageName);

				if (FPaths::DirectoryExists(NodeModulesPath))
				{
					AssetInformationBox->AddSlot().AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString("Already installed in node_modules"))
					];
				}
				else if (FPaths::DirectoryExists(PluginPath))
				{
					AssetInformationBox->AddSlot().AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString("Already installed in plugins"))
					];
				}
				else
				{
					// Add the "Install" button
					AssetInformationBox->AddSlot().AutoHeight()
					[
						SNew(SButton)
						.Text(FText::FromString("Install"))
						.OnClicked(this, &SPackageManagerInstall::OnInstallButtonClicked)
					];
				}
			}
		}
	}

	return FReply::Handled();
}

FSlateColor SPackageManagerInstall::GetRegistryButtonColor(FString RegistryName) const
{
	return RegistryName == SelectedRegistryName ? FSlateColor(FLinearColor::Yellow) : FSlateColor::UseForeground();
}

FSlateColor SPackageManagerInstall::GetScopeButtonColor(FString ScopeName) const
{
	return ScopeName == SelectedScopeName ? FSlateColor(FLinearColor::Yellow) : FSlateColor::UseForeground();
}

FSlateColor SPackageManagerInstall::GetAssetButtonColor(FString AssetName) const
{
	return AssetName == SelectedAssetName ? FSlateColor(FLinearColor::Yellow) : FSlateColor::UseForeground();
}

FReply SPackageManagerInstall::OnInstallButtonClicked()
{
	if (SelectedAsset.IsValid())
	{
		IPythonScriptPlugin* PythonPlugin = IPythonScriptPlugin::Get();

		if (PythonPlugin)
		{
			FString PackageName = SelectedAsset->GetStringField("name");
			FString PluginsDir = FPaths::ProjectPluginsDir();
			TSharedPtr<FJsonObject> MetaObject = SelectedRegistryObject->GetObjectField(TEXT("meta"));
			FString RegistryUrl = MetaObject->GetStringField(TEXT("url"));
			FString Command = FString::Printf(TEXT("%s %s %s %s"), *InstallNpmPath, *PackageName, *PluginsDir,
			                                  *RegistryUrl);

			if (PythonPlugin->ExecPythonCommand(*Command))
			{
				UE_LOG(LogTemp, Log, TEXT("Successfully executed Python command."));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to execute Python command."));
			}
		}

		OnAssetButtonClicked(SelectedAssetName);
	}
	return FReply::Handled();
}

void SPackageManagerInstall::ResetScopeSelection()
{
	if (ScopeSelectBox.IsValid())
	{
		ScopeSelectBox->ClearChildren();
		ScopeSelectBox->AddSlot().AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Select a registry from the left to view details.")))
		];
	}
	ResetAssetSelection();
}

void SPackageManagerInstall::ResetAssetSelection()
{
	if (AssetSelectBox.IsValid())
	{
		AssetSelectBox->ClearChildren();
		AssetSelectBox->AddSlot().AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Select a scope from the left to view details.")))
		];
	}
	if (AssetInformationBox.IsValid())
	{
		AssetInformationBox->ClearChildren();
		AssetInformationBox->AddSlot().AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Select an asset from the left to view details.")))
		];
	}
}
