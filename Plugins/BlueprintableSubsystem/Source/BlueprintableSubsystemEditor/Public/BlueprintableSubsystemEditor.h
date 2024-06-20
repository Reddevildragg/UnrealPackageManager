// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_BPableSubsystem.h"

class FBlueprintableSubsystemEditorModule : public IModuleInterface
{
protected:
    TArray<TSharedPtr<FAssetTypeActions_BPableSubsystemBase>> ActionTypes;
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
