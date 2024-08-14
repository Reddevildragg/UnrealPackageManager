#pragma once

#include "CoreMinimal.h"
#include "UPMPackageJson.h"
#include "Widgets/SCompoundWidget.h"

class SUPMWindow;

class SPackageManagerInstall : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPackageManagerInstall) {}
		SLATE_ARGUMENT(TWeakPtr<SUPMWindow>, ParentWindow)
		SLATE_ARGUMENT(TWeakObjectPtr<UUPMPackageJson>, PackageJsonHandler)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TWeakPtr<SUPMWindow> ParentWindow; // Add this member variable
	TWeakObjectPtr<UUPMPackageJson> PackageJsonHandlerPtr;

	TArray<TSharedPtr<FString>> RegistryNames;
	TSharedPtr<SVerticalBox> RegistrySelectBox;
	TSharedPtr<SVerticalBox> ScopeSelectBox;
	TSharedPtr<SVerticalBox> AssetSelectBox;
	TSharedPtr<SVerticalBox> AssetInformationBox;

	TSharedPtr<FJsonObject> LoadedData;
	TSharedPtr<FJsonObject> SelectedRegistryObject; // Add this member variable
	TArray<TSharedPtr<FJsonValue>> SelectedScopeObject;
	TMap<FString, TSharedPtr<FJsonObject>> AssetMap; // Add this member variable
	TSharedPtr<FJsonObject> SelectedAsset;

	FString SelectedRegistryName;
	FString SelectedScopeName;
	FString SelectedAssetName;

	FReply FetchPackageInformation();
	void LoadCombinedOutput();
	void GenerateRegistryButtons();

	FReply OnRegistryButtonClicked(FString RegistryName);
	FReply OnScopeButtonClicked(FString ScopeName);
	FReply OnAssetButtonClicked(FString AssetName);
	FSlateColor GetRegistryButtonColor(FString RegistryName) const;
	FSlateColor GetScopeButtonColor(FString ScopeName) const;
	FSlateColor GetAssetButtonColor(FString AssetName) const;
	FReply OnInstallButtonClicked();

	void ResetScopeSelection();
	void ResetAssetSelection();
};