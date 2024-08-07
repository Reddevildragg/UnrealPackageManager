#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SLayout2Widget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLayout2Widget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
};