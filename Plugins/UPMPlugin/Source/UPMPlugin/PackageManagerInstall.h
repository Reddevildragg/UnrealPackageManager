#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SPackageManagerInstall : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPackageManagerInstall) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	bool RunNpmCommand(const FString& Command, FString& OutResult, FString& OutError);
};