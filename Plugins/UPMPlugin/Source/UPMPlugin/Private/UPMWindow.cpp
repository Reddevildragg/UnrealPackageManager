#include "UPMWindow.h"

#include "UPMPlugin.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

void SUPMWindow::Construct(const FArguments& InArgs)
{
	Package = MakeShareable(UUPMPackage::LoadOrCreatePackageJson());
	Items.Empty();

	if (Package.IsValid())
	{
		Items.Add(MakeShareable(new FString(FString::Printf(TEXT("Name: %s"), *Package->Name))));
		Items.Add(MakeShareable(new FString(FString::Printf(TEXT("Version: %s"), *Package->Version))));
		for (const auto& Elem : Package->Dependencies)
		{
			Items.Add(MakeShareable(new FString(FString::Printf(TEXT("Dependency: %s -> %s"), *Elem.Key, *Elem.Value))));
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
	];
}

FReply SUPMWindow::OnButtonClicked(int32 ButtonNumber)
{
	UE_LOG(LogTemp, Log, TEXT("Button %d clicked!"), ButtonNumber);
	return FReply::Handled();
}

TSharedRef<ITableRow> SUPMWindow::OnGenerateRowForList(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
	[
		SNew(STextBlock).Text(FText::FromString(*Item))
	];
}
