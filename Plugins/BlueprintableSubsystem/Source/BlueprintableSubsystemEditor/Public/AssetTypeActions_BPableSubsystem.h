// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BPable_GameInstanceSubsystem.h"
#include "GraphEditorSettings.h"
#include "AssetTypeActions/AssetTypeActions_Blueprint.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

class BLUEPRINTABLESUBSYSTEMEDITOR_API FAssetTypeActions_BPableSubsystemBase : public FAssetTypeActions_Base
{
public:
	virtual FColor GetTypeColor() const override { return FColor(63, 126, 255); }
	virtual uint32 GetCategories() override
	{
		static EAssetTypeCategories::Type const AssetCategory = EAssetTypeCategories::Blueprint;
		return AssetCategory;
	}
	virtual const TArray<FText>& GetSubMenus() const override
	{
		static const TArray<FText> SubMenus
		{
			LOCTEXT("BPableSubsystemClassSubMenu", "Subsystems")
		};
		return SubMenus;
	}
};

template <typename SubsystemClass=USubsystem>
class BLUEPRINTABLESUBSYSTEMEDITOR_API FAssetTypeActions_SubsystemTemplate : public FAssetTypeActions_BPableSubsystemBase
{
public:
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override { return SubsystemClass::StaticClass(); }
};

template <typename SubsystemClass>
FText BLUEPRINTABLESUBSYSTEMEDITOR_API FAssetTypeActions_SubsystemTemplate<SubsystemClass>::GetName() const
{
	FString ClassName = SubsystemClass::StaticClass()->GetName();
	int Index;
	ClassName.FindChar('_', Index);
	return FText::FromString(ClassName.RightChop(Index + 1));
}
#undef LOCTEXT_NAMESPACE
