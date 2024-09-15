// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class GoalOrientedActionPlanningEditor : ModuleRules
{
	public GoalOrientedActionPlanningEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
			{ 
				"Core",
				"CoreUObject",
				"Engine",
                "GoalOrientedActionPlanning"
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
