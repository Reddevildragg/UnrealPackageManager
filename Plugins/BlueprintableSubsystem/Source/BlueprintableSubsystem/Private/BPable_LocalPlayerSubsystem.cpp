// Copyright YTSS 2023. All Rights Reserved.

#include "BPable_LocalPlayerSubsystem.h"
#include "BlueprintableSubsystem/BPableSubsystemType.h"

void UBPable_LocalPlayerSubsystem::PostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS)
{
	World->OnWorldBeginPlay.AddUObject(this, &UBPable_LocalPlayerSubsystem::WorldBeginPlay);
}

void UBPable_LocalPlayerSubsystem::StartGameInstance(UGameInstance* GameInstance)
{
	if (GWorld)
	{
		GWorld->OnWorldBeginPlay.AddUObject(this, &UBPable_LocalPlayerSubsystem::WorldBeginPlay);
	}
}

bool UBPable_LocalPlayerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer) && ReceiveShouldCreateSubsystem(Outer);
}

void UBPable_LocalPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	check(!bInitialized)

	// World delegation assign

	FWorldDelegates::OnStartGameInstance.AddUObject(this, &UBPable_LocalPlayerSubsystem::StartGameInstance);
	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UBPable_LocalPlayerSubsystem::PostWorldInitialization);

	FWorldDelegates::OnWorldBeginTearDown.AddUObject(this, &UBPable_LocalPlayerSubsystem::WorldBeginTearingDown);

	ReceiveInitialize();
	SetActive(bActiveDefault);

	bInitialized = true;

	UE_LOG(LogBPableSubsystem, Log, TEXT("%s_Initialize"), *GetName())
}

void UBPable_LocalPlayerSubsystem::Deinitialize()
{
	check(bInitialized);
	ReceiveDeinitialize();
	bInitialized = false;
	UE_LOG(LogBPableSubsystem, Log, TEXT("%s_Deinitialize"), *GetName())
}

FPrimaryAssetId UBPable_LocalPlayerSubsystem::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(PrimaryAssetType_BPableSubsystem, GetFName());
}

bool UBPable_LocalPlayerSubsystem::ReceiveShouldCreateSubsystem_Implementation(UObject* Outer) const
{
	return true;
}
