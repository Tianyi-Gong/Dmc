// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GoalOrientedActionPlanning : ModuleRules
{
	public GoalOrientedActionPlanning(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "GameplayTags",
				"UMG",
                "UnrealEd",
                "GameplayDebugger",
				"RenderCore",
                "InputCore",
                "AIModule", 
				"GameplayAbilities",
                "GameplayTasks",
                "MotionWarping"
            }
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		if (Target.bUseGameplayDebugger || Target.bUseGameplayDebuggerCore)
		{
            PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER_CORE=1");
            PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER=" + (Target.bUseGameplayDebugger ? 1 : 0));
            if (Target.bUseGameplayDebugger || (Target.bUseGameplayDebuggerCore && Target.Configuration != UnrealTargetConfiguration.Shipping))
            {
                PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER_MENU=1");
            }
            else
            {
                PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER_MENU=0");
            }
        }
        else
        {
            PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER_CORE=0");
            PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER=0");
            PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER_MENU=0");
        }
    }
}
