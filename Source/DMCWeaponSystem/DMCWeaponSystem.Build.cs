// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class DMCWeaponSystem : ModuleRules
{
	public DMCWeaponSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
			{ 
				"Core",
				"CoreUObject",
				"Engine",
                "GoalOrientedActionPlanning",
				"GameplayAbilities",
				"GameplayTags",
				"GameplayTasks",
                "AIModule",
                "AnimGraphRuntime"
            });
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				
			});
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				
			});
	}
}
