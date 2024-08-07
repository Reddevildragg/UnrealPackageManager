#include "UPMTabNavigation.h"

#include "UPMWindow.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

void SUPMTabNavigation::Construct(const FArguments& InArgs)
{
	ParentWindow = InArgs._ParentWindow; // Initialize the parent window reference

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SButton)
			.Text(FText::FromString("Settings"))
			.OnClicked_Lambda([this]()
			{
				SelectMode("Settings");
				return FReply::Handled();
			})
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SButton)
			.Text(FText::FromString("Install"))
			.OnClicked_Lambda([this]()
			{
				SelectMode("Install");
				return FReply::Handled();
			})
		]
	];
}

FReply SUPMTabNavigation::SelectMode(const char* Str)
{

	if (TSharedPtr<SUPMWindow> ParentWindowPtr = ParentWindow.Pin())
	{
		ParentWindowPtr->UpdateMode(FString(Str));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Parent window is not valid"));
	}
	return FReply::Handled();
}
