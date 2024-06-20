// Copyright YTSS 2023. All Rights Reserved.

#include "BPable_WorldSubsystem.h"
#include "BlueprintableSubsystem/BPableSubsystemType.h"

bool UBPable_WorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer) && ReceiveShouldCreateSubsystem(Outer);
}

void UBPable_WorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	check(!bInitialized)
	
	ReceiveInitialize();
	SetActive(bActiveDefault);
	
	bInitialized=true;
	
	UE_LOG(LogBPableSubsystem, Log, TEXT("%s_Initialize"), *GetName())
}

void UBPable_WorldSubsystem::Deinitialize()
{
	check(bInitialized)
	ReceiveDeinitialize();
	bInitialized=false;
	UE_LOG(LogBPableSubsystem, Log, TEXT("%s_Deinitialize"), *GetName())
}

FPrimaryAssetId UBPable_WorldSubsystem::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(PrimaryAssetType_BPableSubsystem,GetFName());
}

bool UBPable_WorldSubsystem::ReceiveShouldCreateSubsystem_Implementation(UObject* Outer) const
{
	return true;
}
