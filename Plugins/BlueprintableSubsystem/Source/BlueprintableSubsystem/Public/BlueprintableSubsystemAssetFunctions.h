// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define LOAD_ByPaths_BlueprintableSubsystem(Paths) FBlueprintableSubsystemAssetFunctions::LoadAllBPableSubsystemByPaths(Paths)

#define FIND_AllPluginsDirectory(PluginDirectorys) FBlueprintableSubsystemAssetFunctions::FindAllPluginsDirectory(PluginDirectorys)


class BLUEPRINTABLESUBSYSTEM_API FBlueprintableSubsystemAssetFunctions
{
public:
	
	static void FindAllPluginsDirectory(TSet<FString>& PluginDirectorys);
	static void LoadSubsystemByPaths(UClass* SubsystemClass, const TSet<FString>& Paths);
	static void LoadAllBPableSubsystemByPaths(const TSet<FString>& Paths);
};
