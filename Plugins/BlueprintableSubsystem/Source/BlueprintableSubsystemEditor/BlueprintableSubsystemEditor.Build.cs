// Copyright YTSS 2023. All Rights Reserved.

using UnrealBuildTool;

public class BlueprintableSubsystemEditor : ModuleRules
{
    public BlueprintableSubsystemEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", 
                "AssetTools", "BlueprintableSubsystem",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "BlueprintableSubsystem",
                "UnrealEd",
            }
        );
    }
}