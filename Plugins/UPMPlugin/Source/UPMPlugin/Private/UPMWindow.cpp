#include "UPMWindow.h"

#include "UPMPlugin.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

void SUPMWindow::Construct(const FArguments& InArgs)
{
	Package = FUPMPluginModule::LoadedPackage;
	Items.Empty();
	ScopeTextBoxes.Empty();
	RegistryBoxes.Empty();
	SelectedRegistryName = "";

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
            TSharedPtr<SVerticalBox> RegistryBox = SNew(SVerticalBox)
                + SVerticalBox::Slot()
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
                ]
                + SVerticalBox::Slot()
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
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("Scopes:")))
                ];

            for (const auto& Scope : Registry.Scopes)
            {
                RegistryBox->AddSlot()
                .AutoHeight()
                [
                    SNew(SEditableTextBox)
                    .Text(FText::FromString(*Scope))
                    .OnTextCommitted(this, &SUPMWindow::OnScopeTextCommitted, Registry.Name, Scope)
                ];
            }

            RegistryBoxes.Add(RegistryBox);
        }
	}

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
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Registry Information:"))
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SNew(SSplitter)
			+ SSplitter::Slot()
			.Value(0.3f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SListView<TSharedPtr<SVerticalBox>>)
					.ItemHeight(24)
					.ListItemsSource(&RegistryBoxes)
					.OnGenerateRow(this, &SUPMWindow::OnGenerateRowForRegistry)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SButton)
					.Text(FText::FromString("Select Registry"))
					//.OnClicked(this, &SUPMWindow::OnRegistryButtonClicked, Registry.Name)
				]
			]
			+ SSplitter::Slot()
			.Value(0.7f)
			[
				SNew(SBorder)
				.Padding(15)
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
			.Text(FText::FromString("Save Scopes"))
			.OnClicked(this, &SUPMWindow::OnSaveScopesClicked)
		]
	];
}

FReply SUPMWindow::OnButtonClicked(int32 ButtonNumber)
{
	UE_LOG(LogTemp, Log, TEXT("Button %d clicked!"), ButtonNumber);
	return FReply::Handled();
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
TSharedRef<ITableRow> SUPMWindow::OnGenerateRowForTextBox(TSharedPtr<SWidget> Item, const TSharedRef<STableViewBase>& OwnerTable)
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

TSharedRef<ITableRow> SUPMWindow::OnGenerateRowForRegistry(TSharedPtr<SVerticalBox> Item, const TSharedRef<STableViewBase>& OwnerTable)
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
	SelectedRegistryName = RegistryName;
	// Update the UI to reflect the selected registry
	// This can be done by refreshing the relevant parts of the UI
	return FReply::Handled();
}
