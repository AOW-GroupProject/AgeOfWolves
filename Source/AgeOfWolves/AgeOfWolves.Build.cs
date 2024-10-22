// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AgeOfWolves : ModuleRules
{
	public AgeOfWolves(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.Add(ModuleDirectory);

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "AIModule", "MotionWarping" });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Slate",
            "SlateCore",
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
            "EnhancedInput",
            "Json",
            "JsonUtilities",
            "AnimGraphRuntime",
            "MovieScene",
        });
    }
}
