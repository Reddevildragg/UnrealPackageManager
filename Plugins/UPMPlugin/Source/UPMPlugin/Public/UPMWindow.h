#pragma once

#include "CoreMinimal.h"
#include "UPMPlugin/UPMPackage.h"
#include "Widgets/SCompoundWidget.h"

class SUPMWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUPMWindow) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TSharedPtr<UUPMPackage> Package;
	TArray<TSharedPtr<FString>> Items;
    TArray<TSharedPtr<SVerticalBox>> RegistryBoxes;
	TArray<TSharedPtr<SWidget>> ScopeTextBoxes;
	FString SelectedRegistryName;


	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable);
    TSharedRef<ITableRow> OnGenerateRowForTextBox(TSharedPtr<SWidget> Item, const TSharedRef<STableViewBase>& OwnerTable);
	TSharedRef<ITableRow> OnGenerateRowForRegistry(TSharedPtr<SVerticalBox> Item, const TSharedRef<STableViewBase>& OwnerTable);

	void OnRegistryNameTextCommitted(const FText& NewText, ETextCommit::Type CommitType, FString OldName);
	void OnRegistryUrlTextCommitted(const FText& NewText, ETextCommit::Type CommitType, FString RegistryName);
	void OnScopeTextCommitted(const FText& NewText, ETextCommit::Type CommitType, FString RegistryName,
	                          FString OldScope);
	FReply OnSaveScopesClicked();
	FReply OnRegistryButtonClicked(FString RegistryName);

	FReply OnButtonClicked(int32 ButtonNumber);
};
