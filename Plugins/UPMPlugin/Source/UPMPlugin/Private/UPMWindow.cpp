#include "UPMWindow.h"

#include "UPMPlugin.h"
#include "UPMPlugin/UPMTabNavigation.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

TMap<FString, FString> InitialRegistryNames;
TMap<FString, FString> InitialRegistryUrls;
TMap<FString, TArray<FString>> InitialRegistryScopes;

void SUPMWindow::Construct(const FArguments& InArgs)
{
	Package = FUPMPluginModule::LoadedPackage;
	Items.Empty();
	ScopeTextBoxes.Empty();
	RegistryBoxes.Empty();
	SelectedRegistryName = "";

	StoreInitialValues();
	if (Package.IsValid())
	{
		Items.Add(MakeShareable(new FString(FString::Printf(TEXT("Name: %s"), *Package->Name))));
		Items.Add(MakeShareable(new FString(FString::Printf(TEXT("Version: %s"), *Package->Version))));
		for (const auto& Elem : Package->Dependencies)
		{
			Items.Add(MakeShareable(
				new FString(FString::Printf(TEXT("Dependency: %s -> %s"), *Elem.Key, *Elem.Value))));
		}

		for (const auto& Registry : Package->ScopedRegistries)
		{
			RegistryNames.Add(MakeShareable(new FString(Registry.Name)));
		}
	}

	// Create Layout1 and Layout2 widgets
	SettingsLayout = SNew(SPackageManagerSettings);
	Layout2 = SNew(SLayout2Widget);

	// Create LayoutContainer to hold the layout switching part
	LayoutContainer = SNew(SVerticalBox);

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("UPM Plugin Window")))
		]

		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SNew(SUPMTabNavigation).ParentWindow(SharedThis(this)) // Pass the parent window reference
		]

		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			LayoutContainer.ToSharedRef()
		]

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
			.OnGenerateRow(this, &SUPMWindow::OnGenerateRowForList)
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
				.OnGenerateRow(this, &SUPMWindow::OnGenerateRowForRegistryNames)
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


TSharedRef<ITableRow> SUPMWindow::OnGenerateRowForList(TSharedPtr<FString> Item,
                                                       const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		[
			SNew(STextBlock).Text(FText::FromString(*Item))
		];
}

// Update the `OnGenerateRowForTextBox` method in `UPMWindow.cpp`
TSharedRef<ITableRow> SUPMWindow::OnGenerateRowForTextBox(TSharedPtr<SWidget> Item,
                                                          const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<SWidget>>, OwnerTable)
		[
			Item.ToSharedRef()
		];
}

void SUPMWindow::OnRegistryNameTextCommitted(const FText& NewText, ETextCommit::Type CommitType, FString OldName)
{
	for (auto& Registry : Package->ScopedRegistries)
	{
		if (Registry.Name == OldName)
		{
			Registry.Name = NewText.ToString();
			break;
		}
	}
}

void SUPMWindow::OnRegistryUrlTextCommitted(const FText& NewText, ETextCommit::Type CommitType, FString RegistryName)
{
	for (auto& Registry : Package->ScopedRegistries)
	{
		if (Registry.Name == RegistryName)
		{
			Registry.Url = NewText.ToString();
			break;
		}
	}
}

void SUPMWindow::OnScopeTextCommitted(const FText& NewText, ETextCommit::Type CommitType, FString RegistryName,
                                      FString OldScope)
{
	// Update the scope in the Package object
	for (auto& Registry : Package->ScopedRegistries)
	{
		if (Registry.Name == RegistryName)
		{
			Registry.Scopes.Remove(OldScope);
			Registry.Scopes.Add(NewText.ToString());
			break;
		}
	}
}

TSharedRef<ITableRow> SUPMWindow::OnGenerateRowForRegistry(TSharedPtr<SVerticalBox> Item,
                                                           const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<SVerticalBox>>, OwnerTable)
		[
			Item.ToSharedRef()
		];
}

FReply SUPMWindow::OnSaveScopesClicked()
{
	// Save the updated Package object
	UUPMPackage::SavePackageJson(Package);
	return FReply::Handled();
}

FReply SUPMWindow::OnRegistryButtonClicked(FString RegistryName)
{
	if (RegistryName != SelectedRegistryName)
	{
		ResetFields();
	}

	SelectedRegistryName = RegistryName;

	RegistryDetailsBox->ClearChildren();

	for (const auto& Registry : Package->ScopedRegistries)
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
					.OnTextCommitted(this, &SUPMWindow::OnRegistryNameTextCommitted, Registry.Name)
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
					.OnTextCommitted(this, &SUPMWindow::OnRegistryUrlTextCommitted, Registry.Name)
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
					.OnTextCommitted(this, &SUPMWindow::OnScopeTextCommitted, Registry.Name, Scope)
				];
			}

			RegistryDetailsBox->AddSlot()
			                  .AutoHeight()
			[
				SNew(SButton)
				.Text(FText::FromString("Save Scopes"))
				.OnClicked(this, &SUPMWindow::OnSaveScopesClicked)
			];
			break;
		}
	}

	return FReply::Handled();
}

TSharedRef<ITableRow> SUPMWindow::OnGenerateRowForRegistryNames(TSharedPtr<FString> Item,
                                                                const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		[
			SNew(SButton)
			.Text(FText::FromString(*Item))
			.OnClicked(this, &SUPMWindow::OnRegistryButtonClicked, *Item)
		];
}

// Method to store initial values
void SUPMWindow::StoreInitialValues()
{
	InitialRegistryNames.Empty();
	InitialRegistryUrls.Empty();
	InitialRegistryScopes.Empty();

	for (const auto& Registry : Package->ScopedRegistries)
	{
		InitialRegistryNames.Add(Registry.Name, Registry.Name);
		InitialRegistryUrls.Add(Registry.Name, Registry.Url);
		InitialRegistryScopes.Add(Registry.Name, Registry.Scopes);
	}
}

// Method to reset fields to initial values
void SUPMWindow::ResetFields()
{
	for (auto& Registry : Package->ScopedRegistries)
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

// Call the reset method on tab change
void SUPMWindow::OnTabChanged()
{
	ResetFields();
}

void SUPMWindow::UpdateMode(const FString& Str)
{
	UE_LOG(LogTemp, Log, TEXT("Mode updated to: %s"), *Str);
	if (FCString::Strcmp(*Str, TEXT("Settings")) == 0)
	{
		CurrentLayout = SettingsLayout;
	}
	else
	{
		CurrentLayout = Layout2;
	}

	// Clear existing children of LayoutContainer
	LayoutContainer->ClearChildren();

	// Add the current layout to the container
	LayoutContainer->AddSlot()
	[
		CurrentLayout.ToSharedRef()
	];
}
