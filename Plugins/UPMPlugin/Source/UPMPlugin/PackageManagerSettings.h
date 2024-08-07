#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SPackageManagerSettings : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPackageManagerSettings)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
};
