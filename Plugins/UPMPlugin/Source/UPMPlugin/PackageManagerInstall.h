#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
class UUPMPackage;
class SUPMWindow;

class SPackageManagerInstall : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPackageManagerInstall) {}
		SLATE_ARGUMENT(TWeakPtr<SUPMWindow>, ParentWindow)
		SLATE_ARGUMENT(TSharedPtr<UUPMPackage>, PackageData)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);


private:
	TWeakPtr<SUPMWindow> ParentWindow; // Add this member variable
	TSharedPtr<UUPMPackage> PackageData; // Add this member variable

	FReply OnRunPythonScript();
};