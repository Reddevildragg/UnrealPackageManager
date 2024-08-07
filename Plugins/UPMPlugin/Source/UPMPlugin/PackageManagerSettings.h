#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
class UUPMPackage;
class SUPMWindow;

class SPackageManagerSettings : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPackageManagerSettings){}
		SLATE_ARGUMENT(TWeakPtr<SUPMWindow>, ParentWindow)
		SLATE_ARGUMENT(TSharedPtr<UUPMPackage>, PackageData)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TWeakPtr<SUPMWindow> ParentWindow; // Add this member variable
	TSharedPtr<UUPMPackage> PackageData; // Add this member variable

	TArray<TSharedPtr<FString>> Items;
	TArray<TSharedPtr<SVerticalBox>> RegistryBoxes;
	TArray<TSharedPtr<SWidget>> ScopeTextBoxes;
	TArray<TSharedPtr<FString>> RegistryNames;

	TSharedPtr<SVerticalBox> RegistryDetailsBox;

	FString SelectedRegistryName;

	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable);
	TSharedRef<ITableRow> OnGenerateRowForRegistryNames(TSharedPtr<FString> String, const TSharedRef<STableViewBase>& TableViewBase);
	FReply OnRegistryButtonClicked(FString RegistryName);

	void OnRegistryNameTextCommitted(const FText& NewText, ETextCommit::Type CommitType, FString OldName);
	void OnRegistryUrlTextCommitted(const FText& NewText, ETextCommit::Type CommitType, FString RegistryName);
	void OnScopeTextCommitted(const FText& NewText, ETextCommit::Type CommitType, FString RegistryName,
							  FString OldScope);

	FReply OnSaveScopesClicked();
	void StoreInitialValues();
	void ResetFields();
};
