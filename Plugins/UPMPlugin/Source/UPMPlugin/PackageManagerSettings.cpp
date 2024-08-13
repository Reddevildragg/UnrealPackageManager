#include "PackageManagerSettings.h"
#include "UPMPackage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

TMap<FString, FString> InitialRegistryNames;
TMap<FString, FString> InitialRegistryUrls;
TMap<FString, TArray<FString>> InitialRegistryScopes;
TArray<FScopedRegistry> InitialScopedRegistries = {};

void SPackageManagerSettings::Construct(const FArguments& InArgs)
{
	ParentWindow = InArgs._ParentWindow;
	PackageData = InArgs._PackageData;
	ScopeTextBoxes.Empty();
	RegistryBoxes.Empty();
	SelectedRegistryName = "";

	if (PackageData.IsValid())
	{
		for (const auto& Registry : PackageData->ScopedRegistries)
		{
			RegistryNames.Add(MakeShareable(new FString(Registry.Name)));
		}
	}

	ChildSlot
	[
		SNew(SVerticalBox)
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
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1)
				[
					SAssignNew(RegistryListView, SListView<TSharedPtr<FString>>)
					.ItemHeight(24)
					.ListItemsSource(&RegistryNames)
					.OnGenerateRow(this, &SPackageManagerSettings::OnGenerateRowForRegistryNames)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SButton)
					.Text(FText::FromString("Remove Selected"))
					.OnClicked(this, &SPackageManagerSettings::OnRemoveRegistryButtonClicked)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SButton)
					.Text(FText::FromString("New Registry"))
					.OnClicked(this, &SPackageManagerSettings::OnNewRegistryButtonClicked)
				]
			]
			+ SSplitter::Slot()
			.Value(0.7f)
			[
				SAssignNew(RegistryDetailsBox, SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(15)
				.FillHeight(1)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Select a registry from the left to view details.")))
				]
			]
		]

		+ SVerticalBox::Slot()
		.Padding(0, 10, 0, 0)
		.AutoHeight()
		[
			SNew(SButton)
			.Text(FText::FromString("Save Scopes"))
			.OnClicked(this, &SPackageManagerSettings::OnSaveScopesClicked)
		]
	];

	StoreInitialValues();
}

void SPackageManagerSettings::OnDeselect()
{
	SelectedRegistryName = "";
	SelectedRegistry = nullptr;
}

void SPackageManagerSettings::OnSelect()
{
	ResetFields();
	StoreInitialValues();
}

TSharedRef<ITableRow> SPackageManagerSettings::OnGenerateRowForRegistryNames(TSharedPtr<FString> Item,
                                                                             const TSharedRef<STableViewBase>&
                                                                             OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		[
			SNew(SButton)
			             .Text(FText::FromString(*Item))
			             .ButtonColorAndOpacity(this, &SPackageManagerSettings::GetButtonColor, *Item)
						// Apply highlight effect
			             .OnClicked(this, &SPackageManagerSettings::OnRegistryButtonClicked, *Item)
		];
}

FReply SPackageManagerSettings::OnRegistryButtonClicked(FString RegistryName)
{
	SelectedRegistryName = RegistryName;

	RegistryDetailsBox->ClearChildren();
	SelectedRegistry = nullptr; // Reset the selected registry

	for (auto& Registry : PackageData->ScopedRegistries)
	{
		if (Registry.Name == SelectedRegistryName)
		{
			SelectedRegistry = &Registry; // Store the reference to the selected registry
			break;
		}
	}

	if (SelectedRegistry)
	{
		RegistryDetailsBox->AddSlot()
		                  .AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(0.4)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Registry Name:")))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SEditableTextBox)
				.Text(FText::FromString(SelectedRegistry->Name))
				.OnTextCommitted(this, &SPackageManagerSettings::OnRegistryNameTextCommitted, SelectedRegistry->Name)
			]
		];

		RegistryDetailsBox->AddSlot().AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(0.4)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Registry URL:")))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SEditableTextBox)
				.Text(FText::FromString(SelectedRegistry->Url))
				.OnTextCommitted(this, &SPackageManagerSettings::OnRegistryUrlTextCommitted, SelectedRegistry->Name)
			]
		];

		RegistryDetailsBox->AddSlot()
		                  .AutoHeight().Padding(10)
		[
			SNew(STextBlock)

			.Text(FText::FromString(TEXT("Scopes:")))
		];

		// Create the ScrollBox
		TSharedPtr<SScrollBox> ScrollBox;

		RegistryDetailsBox->AddSlot()
		                  .FillHeight(1)
		[
			SAssignNew(ScrollBox, SScrollBox)
			.Orientation(Orient_Vertical) // Set the scroll orientation to vertical
		];

		// Iterate through the scopes and add each as a slot in the ScrollBox
		for (const auto& Scope : SelectedRegistry->Scopes)
		{
			ScrollBox->AddSlot()
			         .Padding(10, 0, 0, 0)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(0.9f)
				[
					SNew(SEditableTextBox)
					.Text(FText::FromString(Scope))
					.OnTextCommitted(this, &SPackageManagerSettings::OnScopeTextCommitted, SelectedRegistry->Name,
					                 Scope)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("-")))
					.OnClicked(this, &SPackageManagerSettings::OnRemoveScopeButtonClicked, Scope)
				]
				+ SHorizontalBox::Slot()
.AutoWidth()
[
	SNew(SButton)
	.Text(FText::FromString(TEXT("-")))
	.OnClicked(this, &SPackageManagerSettings::OnRemoveScopeButtonClicked, Scope)
]
			];
		}


		RegistryDetailsBox->AddSlot()
		                  .AutoHeight()
		[
			SNew(SButton)
			.Text(FText::FromString("New"))
			.OnClicked(this, &SPackageManagerSettings::OnNewScopeButtonClicked)
		];
	}

	return FReply::Handled();
}

void SPackageManagerSettings::OnRegistryNameTextCommitted(const FText& NewText, ETextCommit::Type CommitType,
                                                          FString OldName)
{
	if (SelectedRegistry && OldName != NewText.ToString())
	{
		SelectedRegistry->Name = NewText.ToString();

		RegistryNames.Empty();
		if (PackageData.IsValid())
		{
			for (const auto& Registry : PackageData->ScopedRegistries)
			{
				RegistryNames.Add(MakeShareable(new FString(Registry.Name)));
			}
		}

		// Refresh the list view to update the UI
		if (RegistryListView.IsValid())
		{
			RegistryListView->RequestListRefresh();
		}
	}
}

void SPackageManagerSettings::OnRegistryUrlTextCommitted(const FText& NewText, ETextCommit::Type CommitType,
                                                         FString RegistryName)
{
	if (SelectedRegistry)
	{
		SelectedRegistry->Url = NewText.ToString();
	}
}

void SPackageManagerSettings::OnScopeTextCommitted(const FText& NewText, ETextCommit::Type CommitType,
                                                   FString RegistryName,
                                                   FString OldScope)
{
	if (SelectedRegistry)
	{
		SelectedRegistry->Scopes.Remove(OldScope);
		SelectedRegistry->Scopes.Add(NewText.ToString());
	}
}

FReply SPackageManagerSettings::OnSaveScopesClicked()
{
	// Save the updated Package object
	UUPMPackage::SavePackageJson(PackageData);

	// Update the initial values to the current state
	StoreInitialValues();

	return FReply::Handled();
}

FReply SPackageManagerSettings::OnNewRegistryButtonClicked()
{
	// Generate a unique name for the new registry
	int32 RegistryCount = PackageData->ScopedRegistries.Num();
	FString NewRegistryName;
	do
	{
		NewRegistryName = FString::Printf(TEXT("Registry%d"), RegistryCount++);
	}
	while (RegistryNames.ContainsByPredicate([&](const TSharedPtr<FString>& Name)
	{
		return *Name == NewRegistryName;
	}));

	// Create a new blank registry
	FScopedRegistry NewRegistry;

	NewRegistry.Name = NewRegistryName;
	NewRegistry.Url = TEXT("");
	NewRegistry.Scopes = {};

	// Add the new registry to the PackageData
	PackageData->ScopedRegistries.Add(NewRegistry);

	// Add the new registry name to the RegistryNames list
	RegistryNames.Add(MakeShareable(new FString(NewRegistry.Name)));

	// Refresh the list view to update the UI
	RegistryListView->RequestListRefresh();

	return FReply::Handled();
}

FSlateColor SPackageManagerSettings::GetButtonColor(FString AssetName) const
{
	return AssetName == SelectedRegistryName ? FSlateColor(FLinearColor::Yellow) : FSlateColor::UseForeground();
}

FReply SPackageManagerSettings::OnNewScopeButtonClicked()
{
	if (SelectedRegistry)
	{
		// Generate a unique name for the new scope
		FString NewScopeName = TEXT("NewScope");
		int32 ScopeCount = 1;
		while (SelectedRegistry->Scopes.Contains(NewScopeName))
		{
			NewScopeName = FString::Printf(TEXT("NewScope%d"), ScopeCount++);
		}

		// Add the new scope to the registry
		SelectedRegistry->Scopes.Add(NewScopeName);

		// Refresh the UI to reflect the new scope
		OnRegistryButtonClicked(SelectedRegistryName);
	}

	return FReply::Handled();
}

FReply SPackageManagerSettings::OnRemoveScopeButtonClicked(FString ScopeName)
{
	if (SelectedRegistry)
	{
		SelectedRegistry->Scopes.Remove(ScopeName);
		OnRegistryButtonClicked(SelectedRegistryName);
	}

	return FReply::Handled();
}

FReply SPackageManagerSettings::OnRemoveRegistryButtonClicked()
{
	if (SelectedRegistry)
	{
		// Remove the selected registry from PackageData
		PackageData->ScopedRegistries.RemoveAll([this](const FScopedRegistry& Registry)
		{
			return Registry.Name == SelectedRegistryName;
		});

		// Remove the selected registry name from the RegistryNames list
		RegistryNames.RemoveAll([this](const TSharedPtr<FString>& Name)
		{
			return *Name == SelectedRegistryName;
		});

		// Clear the selected registry
		SelectedRegistry = nullptr;
		SelectedRegistryName = "";

		// Refresh the list view to update the UI
		RegistryListView->RequestListRefresh();

		// Clear the details box
		RegistryDetailsBox->ClearChildren();
		RegistryDetailsBox->AddSlot()
		                  .AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Select a registry from the left to view details.")))
		];
	}

	return FReply::Handled();
}

void SPackageManagerSettings::StoreInitialValues()
{
	if (PackageData.IsValid())
	{
		InitialScopedRegistries = PackageData->ScopedRegistries;
	}
}

void SPackageManagerSettings::ResetFields()
{
	if (PackageData.IsValid() && InitialScopedRegistries.Num() > 0)
	{
		PackageData->ScopedRegistries = InitialScopedRegistries;

		// Update the RegistryNames list
		RegistryNames.Empty();
		for (const auto& Registry : PackageData->ScopedRegistries)
		{
			RegistryNames.Add(MakeShareable(new FString(Registry.Name)));
		}

		// Refresh the list view to update the UI
		if (RegistryListView.IsValid())
		{
			RegistryListView->RequestListRefresh();
		}

		// Clear the details box
		RegistryDetailsBox->ClearChildren();
		RegistryDetailsBox->AddSlot()
		                  .AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Select a registry from the left to view details.")))
		];
	}
}
