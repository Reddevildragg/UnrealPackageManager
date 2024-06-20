// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "ES_BPableSubsystem_PluginProxy.generated.h"

/**
 * 
 */
UCLASS()
class BLUEPRINTABLESUBSYSTEM_API UES_BPableSubsystem_PluginProxy : public UEngineSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
