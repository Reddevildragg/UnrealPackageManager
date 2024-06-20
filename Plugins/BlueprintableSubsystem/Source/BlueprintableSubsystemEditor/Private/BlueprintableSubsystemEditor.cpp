// Copyright YTSS 2023. All Rights Reserved.

#include "BlueprintableSubsystemEditor.h"

#include "AssetToolsModule.h"

#include "AssetTypeActions_BPableSubsystem.h"

#include "BPable_LocalPlayerSubsystem.h"
#include "BPable_TickableGameInstanceSubsystem.h"
#include "BPable_TickableLocalPlayerSubsystem.h"
#include "BPable_TickableWorldSubsystem.h"
#include "BPable_WorldSubsystem.h"

#include "IAssetTools.h"

#define LOCTEXT_NAMESPACE "FBlueprintableSubsystemEditorModule"

void FBlueprintableSubsystemEditorModule::StartupModule()
{
	// GetAssetTool
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	// Spawn AssetTypeActions
	ActionTypes.Append
	(
		{
			MakeShareable(new FAssetTypeActions_SubsystemTemplate<UBPable_GameInstanceSubsystem>()),
			MakeShareable(new FAssetTypeActions_SubsystemTemplate<UBPable_WorldSubsystem>()),
			MakeShareable(new FAssetTypeActions_SubsystemTemplate<UBPable_LocalPlayerSubsystem>()),
			MakeShareable(new FAssetTypeActions_SubsystemTemplate<UBPable_TickableGameInstanceSubsystem>()),
			MakeShareable(new FAssetTypeActions_SubsystemTemplate<UBPable_TickableWorldSubsystem>()),
			MakeShareable(new FAssetTypeActions_SubsystemTemplate<UBPable_TickableLocalPlayerSubsystem>())
		}
	);

	// Register AssetTypeAction
	for (auto Action : ActionTypes)
	{
		AssetTools.RegisterAssetTypeActions(Action.ToSharedRef());
	}
}

void FBlueprintableSubsystemEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::Get().GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (auto Action : ActionTypes)
		{
			AssetTools.UnregisterAssetTypeActions(Action.ToSharedRef());
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBlueprintableSubsystemEditorModule, BlueprintableSubsystemEditor)
