// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DMC : ModuleRules
{
	public DMC(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayAbilities", "GameplayTags", "GameplayTasks", "GoalOrientedActionPlanning" });

		//PublicIncludePaths.AddRange(new string[] { "GoalOrientedActionPlanning/Public" });
	}
}
