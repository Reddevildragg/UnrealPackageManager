// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BPable_GameInstanceSubsystem.h"
#include "BPable_LocalPlayerSubsystem.h"
#include "BPable_TickableGameInstanceSubsystem.h"
#include "BPable_TickableLocalPlayerSubsystem.h"
#include "BPable_TickableWorldSubsystem.h"
#include "BPable_WorldSubsystem.h"

#include "Kismet2/KismetEditorUtilities.h"
#include "BPableSubsystemFactory.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class BLUEPRINTABLESUBSYSTEMEDITOR_API UBPableSubsystemFactory: public UFactory
{
	GENERATED_BODY()
public:
	UBPableSubsystemFactory()
	{
		bCreateNew=true;
		bEditAfterNew=true;
	}
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
		UObject* Context, FFeedbackContext* Warn, FName CallingContext) override
	{
		// return NewObject<USubsystem>(InParent,InClass,InName,Flags);
		return FKismetEditorUtilities::CreateBlueprint(InClass, InParent, InName, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), CallingContext);
	}

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
		UObject* Context, FFeedbackContext* Warn) override
	{
		return FactoryCreateNew(InClass,InParent,InName,Flags,Context,Warn,NAME_None);
	}
};


UCLASS(HideCategories=Object)
class BLUEPRINTABLESUBSYSTEMEDITOR_API UBPableGameInatanceSubsystemFactory : public UBPableSubsystemFactory
{
	GENERATED_BODY()

public:
	UBPableGameInatanceSubsystemFactory()
	{
		SupportedClass=UBPable_GameInstanceSubsystem::StaticClass();
	}
};

UCLASS(HideCategories=Object)
class BLUEPRINTABLESUBSYSTEMEDITOR_API UBPableWorldSubsystemFactory : public UBPableSubsystemFactory
{
	GENERATED_BODY()

public:
	UBPableWorldSubsystemFactory()
	{
		SupportedClass=UBPable_WorldSubsystem::StaticClass();
	}
};

UCLASS(HideCategories=Object)
class BLUEPRINTABLESUBSYSTEMEDITOR_API UBPableLocalPlayerSubsystemFactory : public UBPableSubsystemFactory
{
	GENERATED_BODY()

public:
	UBPableLocalPlayerSubsystemFactory()
	{
		SupportedClass=UBPable_LocalPlayerSubsystem::StaticClass();
	}
};



UCLASS(HideCategories=Object)
class BLUEPRINTABLESUBSYSTEMEDITOR_API UBPableTickableGameInatanceSubsystemFactory : public UBPableSubsystemFactory
{
	GENERATED_BODY()

public:
	UBPableTickableGameInatanceSubsystemFactory()
	{
		SupportedClass=UBPable_TickableGameInstanceSubsystem::StaticClass();
	}
};

UCLASS(HideCategories=Object)
class BLUEPRINTABLESUBSYSTEMEDITOR_API UBPableTickableWorldSubsystemFactory : public UBPableSubsystemFactory
{
	GENERATED_BODY()

public:
	UBPableTickableWorldSubsystemFactory()
	{
		SupportedClass=UBPable_TickableWorldSubsystem::StaticClass();
	}
};

UCLASS(HideCategories=Object)
class BLUEPRINTABLESUBSYSTEMEDITOR_API UBPableTickableLocalPlayerSubsystemFactory : public UBPableSubsystemFactory
{
	GENERATED_BODY()

public:
	UBPableTickableLocalPlayerSubsystemFactory()
	{
		SupportedClass=UBPable_TickableLocalPlayerSubsystem::StaticClass();
	}
};