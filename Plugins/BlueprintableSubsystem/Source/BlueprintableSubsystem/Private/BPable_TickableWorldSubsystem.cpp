// Copyright YTSS 2023. All Rights Reserved.

#include "BPable_TickableWorldSubsystem.h"
#include "BlueprintableSubsystem/TickableSubsystem.h"

void UBPable_TickableWorldSubsystem::Tick(float DeltaTime)
{
	checkf(bInitialized,
	       TEXT(
		       "Ticking should have been disabled for an uninitialized subsystem : remember to call IsInitialized in the subsystem's IsTickable, IsTickableInEditor and/or IsTickableWhenPaused implementation"
	       ));
	ReceiveTick(DeltaTime);
}

ETickableTickType UBPable_TickableWorldSubsystem::GetTickableTickType() const
{
	return IsTemplate() ? ETickableTickType::Never : FTickableSubsystem::GetTickableTickType();
}

bool UBPable_TickableWorldSubsystem::IsTickable() const
{
	return FTickableSubsystem::IsTickable() && !IsTemplate() && bInitialized && IsActive();
}

void UBPable_TickableWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SetTickEnabled(bIsTickEnabled);
	SetTickableWhenPaused(bIsTickableWhenPaused);
}
