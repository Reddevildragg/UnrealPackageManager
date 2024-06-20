// Copyright YTSS 2023. All Rights Reserved.


#include "ES_BPableSubsystem_PluginProxy.h"
#include "BlueprintableSubsystemAssetFunctions.h"
#include "Engine/AssetManager.h"

void UES_BPableSubsystem_PluginProxy::Initialize(FSubsystemCollectionBase& Collection)
{
	// UAssetManager::Get();
	
	TSet<FString> PreparedPaths;
	FIND_AllPluginsDirectory(PreparedPaths);
	LOAD_ByPaths_BlueprintableSubsystem(PreparedPaths);
}
