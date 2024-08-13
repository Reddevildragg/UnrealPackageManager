#pragma once

#include "CoreMinimal.h"

class SUPMWindow;

class SUPMTabNavigation : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUPMTabNavigation){}
		SLATE_ARGUMENT(TWeakPtr<SUPMWindow>, ParentWindow)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);


private:
	TWeakPtr<SUPMWindow> ParentWindow; // Add this member variable

	FName SelectedTab;

	TSharedRef<SButton> CreateButton(FName ModeName, FString ButtonText);
	FReply SelectMode(FName mode);
	FSlateColor GetButtonColor(FName AssetName) const;

};
