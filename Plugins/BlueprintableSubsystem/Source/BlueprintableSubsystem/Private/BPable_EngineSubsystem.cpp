// Copyright YTSS 2023. All Rights Reserved.

#include "BPable_EngineSubsystem.h"
#include "BlueprintableSubsystem/BPableSubsystemType.h"

bool UBPable_EngineSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer) && ReceiveShouldCreateSubsystem(Outer);
}

void UBPable_EngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ReceiveInitialize();
	UE_LOG(LogBPableSubsystem, Warning, TEXT("%s_Initialize"), *GetName())
}

void UBPable_EngineSubsystem::Deinitialize()
{
	ReceiveDeinitialize();
	Super::Deinitialize();
	UE_LOG(LogBPableSubsystem, Warning, TEXT("%s_Deinitialize"), *GetName())
}

bool UBPable_EngineSubsystem::ReceiveShouldCreateSubsystem_Implementation(UObject* Outer) const
{
	return true;
}
