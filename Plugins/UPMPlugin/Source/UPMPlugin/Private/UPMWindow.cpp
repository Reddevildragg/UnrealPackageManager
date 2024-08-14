#include "UPMWindow.h"

#include "UPMPlugin.h"
#include "UPMPlugin/UPMTabNavigation.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

void SUPMWindow::Construct(const FArguments& InArgs)
{
	UE_LOG(LogTemp, Log, TEXT("Constructing UPM Window"));

	// Create and initialize the UUPMPackageJson object
	PackageJsonHandler = MakeShareable(NewObject<UUPMPackageJson>(), [](UUPMPackageJson* Ptr) { Ptr->ConditionalBeginDestroy(); });
	if (PackageJsonHandler.IsValid())
	{
		PackageJsonHandler->LoadOrCreatePackageJson();
		if (PackageJsonHandler->GetJsonObject().IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("Successfully loaded or created package.json"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load package.json data"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create UUPMPackageJson instance"));
	}

	// Create Layout1 and Layout2 widgets
	SettingsLayout = SNew(SPackageManagerSettings).ParentWindow(SharedThis(this)).PackageJsonHandler(TWeakObjectPtr<UUPMPackageJson>(PackageJsonHandler.Get()));
	InstallLayout = SNew(SPackageManagerInstall).ParentWindow(SharedThis(this)).PackageJsonHandler(TWeakObjectPtr<UUPMPackageJson>(PackageJsonHandler.Get()));

	// Create LayoutContainer to hold the layout switching part
	LayoutContainer = SNew(SVerticalBox);

	UE_LOG(LogTemp, Log, TEXT("Layouits Loaded"));
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().Padding(10).FillHeight(1)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
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
		]
	];

	UpdateMode("Settings");
}


void SUPMWindow::UpdateMode(const FString& Str)
{
	UE_LOG(LogTemp, Log, TEXT("Mode updated to: %s"), *Str);

	SettingsLayout->OnDeselect();

	if (FCString::Strcmp(*Str, TEXT("Settings")) == 0)
	{
		CurrentLayout = SettingsLayout;
		SettingsLayout->OnSelect();
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

TSharedPtr<FPackageJson> SUPMWindow::GetPackageJsonData() const
{
	return PackageJsonHandler.IsValid() ? PackageJsonHandler->GetJsonObject() : nullptr;
}