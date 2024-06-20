// Copyright YTSS 2023. All Rights Reserved.

#include "BPable_GameInstanceSubsystem.h"
#include "BlueprintableSubsystem/BPableSubsystemType.h"

void UBPable_GameInstanceSubsystem::PostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS)
{
	World->OnWorldBeginPlay.AddUObject(this, &UBPable_GameInstanceSubsystem::WorldBeginPlay);
}

void UBPable_GameInstanceSubsystem::StartGameInstance(UGameInstance* GameInstance)
{
	if (GWorld)
	{
		GWorld->OnWorldBeginPlay.AddUObject(this, &UBPable_GameInstanceSubsystem::WorldBeginPlay);
	}
}

bool UBPable_GameInstanceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer) && ReceiveShouldCreateSubsystem(Outer);
}

void UBPable_GameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	check(!bInitialized)
	
	// World delegation assign
	
	// FWorldDelegates::OnStartGameInstance.AddUObject(this, &UBPable_GameInstanceSubsystem::StartGameInstance);
	FWorldDelegates::OnPostWorldInitialization.
		AddUObject(this, &UBPable_GameInstanceSubsystem::PostWorldInitialization);
	FWorldDelegates::OnWorldBeginTearDown.AddUObject(this,&UBPable_GameInstanceSubsystem::WorldBeginTearingDown);

	ReceiveInitialize();
	SetActive(bActiveDefault);

	bInitialized = true;

	UE_LOG(LogBPableSubsystem, Log, TEXT("%s_Initialize"), *GetName())
}

void UBPable_GameInstanceSubsystem::Deinitialize()
{
	check(bInitialized)
	ReceiveDeinitialize();
	bInitialized = false;
	UE_LOG(LogBPableSubsystem, Log, TEXT("%s_Deinitialize"), *GetName())
}

FPrimaryAssetId UBPable_GameInstanceSubsystem::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(PrimaryAssetType_BPableSubsystem,GetFName());
}

bool UBPable_GameInstanceSubsystem::ReceiveShouldCreateSubsystem_Implementation(UObject* Outer) const
{
	return true;
}
