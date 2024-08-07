#include "PackageManagerSettings.h"
#include "UPMPackage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

TMap<FString, FString> InitialRegistryNames;
TMap<FString, FString> InitialRegistryUrls;
TMap<FString, TArray<FString>> InitialRegistryScopes;

void SPackageManagerSettings::Construct(const FArguments& InArgs)
{
	ParentWindow = InArgs._ParentWindow;
	PackageData = InArgs._PackageData;

	Items.Empty();
	ScopeTextBoxes.Empty();
	RegistryBoxes.Empty();
	SelectedRegistryName = "";

	// Log to check if PackageData is set
	// Log to check if PackageData is set
	if (PackageData.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("PackageData is valid"));
		UE_LOG(LogTemp, Log, TEXT("Package Name: %s"), *PackageData->Name);
		UE_LOG(LogTemp, Log, TEXT("Package Version: %s"), *PackageData->Version);

		// Log dependencies
		for (const auto& Elem : PackageData->Dependencies)
		{
			UE_LOG(LogTemp, Log, TEXT("Dependency: %s -> %s"), *Elem.Key, *Elem.Value);
		}

		// Log scoped registries
		for (const auto& Registry : PackageData->ScopedRegistries)
		{
			UE_LOG(LogTemp, Log, TEXT("Registry Name: %s"), *Registry.Name);
			UE_LOG(LogTemp, Log, TEXT("Registry URL: %s"), *Registry.Url);
			for (const auto& Scope : Registry.Scopes)
			{
				UE_LOG(LogTemp, Log, TEXT("Scope: %s"), *Scope);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PackageData is not valid"));
	}

	if (PackageData.IsValid())
	{
		Items.Add(MakeShareable(new FString(FString::Printf(TEXT("Name: %s"), *PackageData->Name))));
		Items.Add(MakeShareable(new FString(FString::Printf(TEXT("Version: %s"), *PackageData->Version))));
		for (const auto& Elem : PackageData->Dependencies)
		{
			Items.Add(MakeShareable(
				new FString(FString::Printf(TEXT("Dependency: %s -> %s"), *Elem.Key, *Elem.Value))));
		}

		for (const auto& Registry : PackageData->ScopedRegistries)
		{
			RegistryNames.Add(MakeShareable(new FString(Registry.Name)));
		}
	}

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Package Information:"))
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SListView<TSharedPtr<FString>>)
			.ItemHeight(24)
			.ListItemsSource(&Items)
			.OnGenerateRow(this, &SPackageManagerSettings::OnGenerateRowForList)
		]
		+ SVerticalBox::Slot()
		.Padding(0, 10, 0, 0)
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Registry Information:"))
		]
		+ SVerticalBox::Slot()
		.Padding(0, 10, 0, 0)
		.FillHeight(1.0f)
		[
			SNew(SSplitter)
			+ SSplitter::Slot()
			.Value(0.3f)
			[
				SNew(SListView<TSharedPtr<FString>>)
				.ItemHeight(24)
				.ListItemsSource(&RegistryNames)
				.OnGenerateRow(this, &SPackageManagerSettings::OnGenerateRowForRegistryNames)
			]
			+ SSplitter::Slot()
			.Value(0.7f)
			[
				SAssignNew(RegistryDetailsBox, SVerticalBox)
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

TSharedRef<ITableRow> SPackageManagerSettings::OnGenerateRowForList(TSharedPtr<FString> Item,
                                                                    const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		[
			SNew(STextBlock).Text(FText::FromString(*Item))
		];
}

TSharedRef<ITableRow> SPackageManagerSettings::OnGenerateRowForRegistryNames(TSharedPtr<FString> Item,
                                                                             const TSharedRef<STableViewBase>&
                                                                             OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		[
			SNew(SButton)
			.Text(FText::FromString(*Item))
			.OnClicked(this, &SPackageManagerSettings::OnRegistryButtonClicked, *Item)
		];
}

FReply SPackageManagerSettings::OnRegistryButtonClicked(FString RegistryName)
{
	if (RegistryName != SelectedRegistryName)
	{
		ResetFields();
	}

	SelectedRegistryName = RegistryName;

	RegistryDetailsBox->ClearChildren();

	for (const auto& Registry : PackageData->ScopedRegistries)
	{
		if (Registry.Name == SelectedRegistryName)
		{
			RegistryDetailsBox->AddSlot()
			                  .AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Registry Name:")))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(SEditableTextBox)
					.Text(FText::FromString(Registry.Name))
					.OnTextCommitted(this, &SPackageManagerSettings::OnRegistryNameTextCommitted, Registry.Name)
				]
			];

			RegistryDetailsBox->AddSlot()
			                  .AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Registry URL:")))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(SEditableTextBox)
					.Text(FText::FromString(Registry.Url))
					.OnTextCommitted(this, &SPackageManagerSettings::OnRegistryUrlTextCommitted, Registry.Name)
				]
			];

			RegistryDetailsBox->AddSlot()
			                  .AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Scopes:")))
			];

			for (const auto& Scope : Registry.Scopes)
			{
				RegistryDetailsBox->AddSlot()
				                  .AutoHeight()
				[
					SNew(SEditableTextBox)
					.Text(FText::FromString(*Scope))
					.OnTextCommitted(this, &SPackageManagerSettings::OnScopeTextCommitted, Registry.Name, Scope)
				];
			}

			RegistryDetailsBox->AddSlot()
			                  .AutoHeight()
			[
				SNew(SButton)
				.Text(FText::FromString("Save Scopes"))
				.OnClicked(this, &SPackageManagerSettings::OnSaveScopesClicked)
			];
			break;
		}
	}

	return FReply::Handled();
}

void SPackageManagerSettings::OnRegistryNameTextCommitted(const FText& NewText, ETextCommit::Type CommitType,
                                                          FString OldName)
{
	for (auto& Registry : PackageData->ScopedRegistries)
	{
		if (Registry.Name == OldName)
		{
			Registry.Name = NewText.ToString();
			break;
		}
	}
}

void SPackageManagerSettings::OnRegistryUrlTextCommitted(const FText& NewText, ETextCommit::Type CommitType,
                                                         FString RegistryName)
{
	for (auto& Registry : PackageData->ScopedRegistries)
	{
		if (Registry.Name == RegistryName)
		{
			Registry.Url = NewText.ToString();
			break;
		}
	}
}

void SPackageManagerSettings::OnScopeTextCommitted(const FText& NewText, ETextCommit::Type CommitType,
                                                   FString RegistryName,
                                                   FString OldScope)
{
	// Update the scope in the Package object
	for (auto& Registry : PackageData->ScopedRegistries)
	{
		if (Registry.Name == RegistryName)
		{
			Registry.Scopes.Remove(OldScope);
			Registry.Scopes.Add(NewText.ToString());
			break;
		}
	}
}

FReply SPackageManagerSettings::OnSaveScopesClicked()
{
	// Save the updated Package object
	UUPMPackage::SavePackageJson(PackageData);
	return FReply::Handled();
}

// Method to store initial values
void SPackageManagerSettings::StoreInitialValues()
{
	InitialRegistryNames.Empty();
	InitialRegistryUrls.Empty();
	InitialRegistryScopes.Empty();

	for (const auto& Registry : PackageData->ScopedRegistries)
	{
		InitialRegistryNames.Add(Registry.Name, Registry.Name);
		InitialRegistryUrls.Add(Registry.Name, Registry.Url);
		InitialRegistryScopes.Add(Registry.Name, Registry.Scopes);
	}
}

// Method to reset fields to initial values
void SPackageManagerSettings::ResetFields()
{
	for (auto& Registry : PackageData->ScopedRegistries)
	{
		if (InitialRegistryNames.Contains(Registry.Name))
		{
			Registry.Name = InitialRegistryNames[Registry.Name];
		}
		if (InitialRegistryUrls.Contains(Registry.Name))
		{
			Registry.Url = InitialRegistryUrls[Registry.Name];
		}
		if (InitialRegistryScopes.Contains(Registry.Name))
		{
			Registry.Scopes = InitialRegistryScopes[Registry.Name];
		}
	}

	// Refresh the UI to reflect the reset values
	OnRegistryButtonClicked(SelectedRegistryName);
}
