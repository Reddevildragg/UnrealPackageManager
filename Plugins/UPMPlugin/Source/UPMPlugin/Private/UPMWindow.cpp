#include "UPMWindow.h"

#include "UPMPlugin.h"
#include "UPMPlugin/UPMTabNavigation.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

void SUPMWindow::Construct(const FArguments& InArgs)
{
	Package = FUPMPluginModule::LoadedPackage;

	// Create Layout1 and Layout2 widgets
	SettingsLayout = SNew(SPackageManagerSettings).ParentWindow(SharedThis(this)).PackageData(Package);
	InstallLayout = SNew(SPackageManagerInstall).ParentWindow(SharedThis(this)).PackageData(Package);

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
		.AutoHeight()
		[
			SNew(SUPMTabNavigation).ParentWindow(SharedThis(this)) // Pass the parent window reference
		]

		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			LayoutContainer.ToSharedRef()
		]
	];
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
		CurrentLayout = InstallLayout;
	}

	// Clear existing children of LayoutContainer
	LayoutContainer->ClearChildren();

	// Add the current layout to the container
	LayoutContainer->AddSlot()
	[
		CurrentLayout.ToSharedRef()
	];
}
