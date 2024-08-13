#include "PackageManagerInstall.h"
#include "UPMPackage.h"
#include "Widgets/Input/SButton.h"
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
		.AutoHeight()
		[
			SNew(SButton)
			.Text(FText::FromString("Run Python Script"))
			.OnClicked(this, &SPackageManagerInstall::FetchPackageInformation)
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
				.Padding(15)
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
				.Padding(15)
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
				.Padding(15)
				.AutoHeight()
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Select a registry from the left to view details.")))
				]
			]
		]
	];
}

FReply SPackageManagerInstall::FetchPackageInformation()
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

                // Save the name and URL of the registry inside a meta tag
                TSharedPtr<FJsonObject> MetaJsonObject = MakeShareable(new FJsonObject);
                MetaJsonObject->SetStringField(TEXT("name"), Registry.Name);
                MetaJsonObject->SetStringField(TEXT("url"), Registry.Url);
                RegistryJsonObject->SetObjectField(TEXT("meta"), MetaJsonObject);

                // Save all scope information inside a scopes tag
                TSharedPtr<FJsonObject> ScopesJsonObject = MakeShareable(new FJsonObject);
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
        FString CombinedOutputFilePath = FPlatformMisc::GetEnvironmentVariable(TEXT("APPDATA")) / TEXT("MyApp/combined_output.json");
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
	FString CombinedOutputFilePath = FPlatformMisc::GetEnvironmentVariable(TEXT("APPDATA")) / TEXT("MyApp/combined_output.json");
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
                    .ButtonColorAndOpacity(this, &SPackageManagerInstall::GetRegistryButtonColor, RegistryName) // Apply highlight effect
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
                            .ButtonColorAndOpacity(this, &SPackageManagerInstall::GetScopeButtonColor, Scope) // Apply highlight effect
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
						.ButtonColorAndOpacity(this, &SPackageManagerInstall::GetAssetButtonColor, AssetName) // Apply highlight effect
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

    return FReply::Handled();
}

FSlateColor SPackageManagerInstall::GetRegistryButtonColor(FString RegistryName)const
{
	return RegistryName == SelectedRegistryName ? FSlateColor(FLinearColor::Yellow) : FSlateColor::UseForeground();
}

FSlateColor SPackageManagerInstall::GetScopeButtonColor(FString ScopeName)const
{
	return ScopeName == SelectedScopeName ? FSlateColor(FLinearColor::Yellow) : FSlateColor::UseForeground();
}

FSlateColor SPackageManagerInstall::GetAssetButtonColor(FString AssetName)const
{
	return AssetName == SelectedAssetName ? FSlateColor(FLinearColor::Yellow) : FSlateColor::UseForeground();
}

FReply SPackageManagerInstall::OnInstallButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("install"));
	if (SelectedAsset.IsValid())
	{
		IPythonScriptPlugin* PythonPlugin = IPythonScriptPlugin::Get();

		if (PythonPlugin)
		{

			FString PackageName = SelectedAsset->GetStringField("name");
			FString PluginsDir = FPaths::ProjectPluginsDir();
			TSharedPtr<FJsonObject> MetaObject = SelectedRegistryObject->GetObjectField(TEXT("meta"));
			FString RegistryUrl = MetaObject->GetStringField(TEXT("url"));
			FString ScriptPath = FPaths::ProjectPluginsDir() / TEXT("UPMPlugin/install_package.py");

			FString Command = FString::Printf(TEXT("%s %s %s %s"), *ScriptPath, *PackageName, *PluginsDir, *RegistryUrl);

			if (PythonPlugin->ExecPythonCommand(*Command))
			{
				UE_LOG(LogTemp, Log, TEXT("Successfully executed Python command."));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to execute Python command."));
			}
		}
	}
	return FReply::Handled();
}