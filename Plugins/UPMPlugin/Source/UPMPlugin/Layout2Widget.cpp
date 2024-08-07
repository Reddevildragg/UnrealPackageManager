#include "Layout2Widget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

void SLayout2Widget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("This is Layout 2"))
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SButton)
			.Text(FText::FromString("Switch to Layout 1"))
			.OnClicked_Lambda([]() {
				// You can use the function below for changing layouts or communicate with the parent widget
				return FReply::Handled();
			})
		]
	];
}