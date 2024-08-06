#pragma once

#include "CoreMinimal.h"
#include "UPMPlugin/UPMPackage.h"
#include "Widgets/SCompoundWidget.h"

class SUPMWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUPMWindow) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TSharedPtr<UUPMPackage> Package;
	TArray<TSharedPtr<FString>> Items;

	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable);
	FReply OnButtonClicked(int32 ButtonNumber);
};
