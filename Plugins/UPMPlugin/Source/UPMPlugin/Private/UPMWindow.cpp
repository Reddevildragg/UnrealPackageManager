#include "UPMWindow.h"

#include "UPMPlugin.h"
#include "UPMPlugin/UPMTabNavigation.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

void SUPMWindow::Construct(const FArguments& InArgs)
{
	UE_LOG(LogTemp, Log, TEXT("Constructing UPM Window"));

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

	TWeakObjectPtr<UUPMPackageJson> WeakPackageJsonHandler = PackageJsonHandler.Get();
	SettingsLayout = SNew(SPackageManagerSettings).ParentWindow(SharedThis(this)).PackageJsonHandler(WeakPackageJsonHandler);
	InstallLayout = SNew(SPackageManagerInstall).ParentWindow(SharedThis(this)).PackageJsonHandler(WeakPackageJsonHandler);

	LayoutContainer = SNew(SVerticalBox);

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().Padding(10).FillHeight(1)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(STextBlock).Text(FText::FromString(TEXT("UPM Plugin Window")))
			]
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(SUPMTabNavigation).ParentWindow(SharedThis(this))
			]
			+ SVerticalBox::Slot().FillHeight(1.0f)
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

    if (Str == "Settings")
    {
        CurrentLayout = SettingsLayout;
        SettingsLayout->OnSelect();
    }
    else
    {
        CurrentLayout = InstallLayout;
    }

    LayoutContainer->ClearChildren();
    LayoutContainer->AddSlot()[CurrentLayout.ToSharedRef()];
}