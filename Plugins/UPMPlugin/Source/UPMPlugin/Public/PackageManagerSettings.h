#pragma once

#include "CoreMinimal.h"
#include "UPMPackageJson.h"
#include "Widgets/SCompoundWidget.h"

class SUPMWindow;
class SPackageManagerSettings : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPackageManagerSettings){}
		SLATE_ARGUMENT(TWeakPtr<SUPMWindow>, ParentWindow)
		SLATE_ARGUMENT(TWeakObjectPtr<UUPMPackageJson>, PackageJsonHandler)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void OnDeselect();
	void OnSelect();

	virtual ~SPackageManagerSettings() override
	{
		ParentWindow.Reset();
		PackageJsonHandlerPtr.Reset();
	}

private:
	TWeakPtr<SUPMWindow> ParentWindow; // Add this member variable
	TWeakObjectPtr<UUPMPackageJson> PackageJsonHandlerPtr;

	TArray<TSharedPtr<SVerticalBox>> RegistryBoxes;
	TArray<TSharedPtr<SWidget>> ScopeTextBoxes;
	TArray<TSharedPtr<FString>> RegistryNames;
	TSharedPtr<SListView<TSharedPtr<FString>>> RegistryListView;

	TSharedPtr<SVerticalBox> RegistryDetailsBox;

	FString SelectedRegistryName;
	FScopedRegistry* SelectedRegistry = nullptr; // Ensure this is a non-const pointer


	TSharedRef<ITableRow> OnGenerateRowForRegistryNames(TSharedPtr<FString> String, const TSharedRef<STableViewBase>& TableViewBase);
	FReply OnRegistryButtonClicked(FString RegistryName);

	void OnRegistryNameTextCommitted(const FText& NewText, ETextCommit::Type CommitType, FString OldName);
	void OnRegistryUrlTextCommitted(const FText& NewText, ETextCommit::Type CommitType, FString RegistryName);
	void OnScopeTextCommitted(const FText& NewText, ETextCommit::Type CommitType, FString RegistryName,
							  FString OldScope);

	FReply OnSaveScopesClicked();

	FSlateColor GetButtonColor(FString AssetName) const;

	FReply OnNewRegistryButtonClicked();
	FReply OnRemoveRegistryButtonClicked();

	FReply OnNewScopeButtonClicked();
	FReply OnRemoveScopeButtonClicked(FString ScopeName);

	void StoreInitialValues();
	void ResetFields();
	TArray<FScopedRegistry> InitialScopedRegistries ; // Add this member variable

};
