// Copyright YTSS 2023. All Rights Reserved.

#include "BPable_TickableLocalPlayerSubsystem.h"
#include "BlueprintableSubsystem/TickableSubsystem.h"

void UBPable_TickableLocalPlayerSubsystem::Tick(float DeltaSeconds)
{
	checkf(bInitialized,
	       TEXT(
		       "Ticking should have been disabled for an uninitialized subsystem : remember to call IsInitialized in the subsystem's IsTickable, IsTickableInEditor and/or IsTickableWhenPaused implementation"
	       ));
	ReceiveTick(DeltaSeconds);
}

ETickableTickType UBPable_TickableLocalPlayerSubsystem::GetTickableTickType() const
{
	return IsTemplate() ? ETickableTickType::Never : FTickableSubsystem::GetTickableTickType();
}

bool UBPable_TickableLocalPlayerSubsystem::IsTickable() const
{
	return FTickableSubsystem::IsTickable() && !IsTemplate() && bInitialized && IsActive();
}

void UBPable_TickableLocalPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SetTickEnabled(bIsTickEnabled);
	SetTickableWhenPaused(bIsTickableWhenPaused);
}
