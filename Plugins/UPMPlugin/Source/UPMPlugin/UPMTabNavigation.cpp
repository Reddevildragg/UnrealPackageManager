#include "UPMTabNavigation.h"

#include "UPMWindow.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SUniformGridPanel.h"

TArray<TPair<FName, FString>> ButtonInfoArray = {
	TPair<FName, FString>(FName("Settings"), "Settings"),
	TPair<FName, FString>(FName("Install"), "Install"),
};

void SUPMTabNavigation::Construct(const FArguments& InArgs)
{
	ParentWindow = InArgs._ParentWindow; // Initialize the parent window reference

	// Iterate through the array and add buttons dynamically
	TSharedPtr<SHorizontalBox> HorizontalBox = SNew(SHorizontalBox);

	for (int32 Index = 0; Index < ButtonInfoArray.Num(); ++Index)
	{
		HorizontalBox->AddSlot()
			.FillWidth(1)
			.Padding(FMargin(5.0f))  // Optional: Adjust padding between buttons
			[
				CreateButton(ButtonInfoArray[Index].Key, ButtonInfoArray[Index].Value)
			];
	}

	// Assign the constructed HorizontalBox to the ChildSlot
	ChildSlot
	[
		HorizontalBox.ToSharedRef()
	];

	SelectMode(ButtonInfoArray[0].Key);
}

TSharedRef<SButton> SUPMTabNavigation::CreateButton(FName ModeName, FString ButtonText)
{
	return SNew(SButton)
		.Text(FText::FromString(ButtonText))
		.ButtonColorAndOpacity(this, &SUPMTabNavigation::GetButtonColor,ModeName) // Apply highlight effect
		.OnClicked_Lambda([this, ModeName]()
		{
			SelectMode(ModeName);
			return FReply::Handled();
		});
}

FReply SUPMTabNavigation::SelectMode(FName mode)
{
	if (TSharedPtr<SUPMWindow> ParentWindowPtr = ParentWindow.Pin())
	{
		SelectedTab = mode;
		ParentWindowPtr->UpdateMode(mode.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Parent window is not valid"));
	}
	return FReply::Handled();
}

FSlateColor SUPMTabNavigation::GetButtonColor(FName AssetName) const
{
	return AssetName == SelectedTab ? FSlateColor(FLinearColor::Yellow) : FSlateColor::UseForeground();
}
