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

	FReply SelectMode(const char* Str);

private:
	TWeakPtr<SUPMWindow> ParentWindow; // Add this member variable
};
