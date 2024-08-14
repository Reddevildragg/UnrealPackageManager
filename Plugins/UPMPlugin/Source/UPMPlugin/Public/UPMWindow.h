#pragma once

#include "CoreMinimal.h"
#include "UPMPlugin/PackageManagerInstall.h"
#include "UPMPlugin/PackageManagerSettings.h"
#include "UPMPlugin/UPMPackageJson.h"
#include "Widgets/SCompoundWidget.h"

class SUPMWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUPMWindow) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void UpdateMode(const FString& Str);

private:
	TSharedPtr<UUPMPackageJson> PackageJsonHandler;

	//Menu Items
	TSharedPtr<SPackageManagerInstall> InstallLayout;
	TSharedPtr<SPackageManagerSettings> SettingsLayout;

	TSharedPtr<SWidget> CurrentLayout;
	TSharedPtr<SVerticalBox> LayoutContainer;
};
