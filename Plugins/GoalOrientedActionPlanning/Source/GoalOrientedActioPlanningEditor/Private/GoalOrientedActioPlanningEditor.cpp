// Fill out your copyright notice in the Description page of Project Settings.

#include "GoalOrientedActionPlanningEditor.h"
#include "ISettingsModule.h"
#include "Logging.h"

#include "GoapWorldStateRuntimeSettings.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FGoalOrientedActionPlanningEditor"

void FGoalOrientedActionPlanningEditor::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FGoalOrientedActionPlanningEditor::OnPostEngineInit);
}

void FGoalOrientedActionPlanningEditor::ShutdownModule()
{
	if (UObjectInitialized())
	{
		UnregisterSettings();
	}
}


void FGoalOrientedActionPlanningEditor::OnPostEngineInit()
{
	RegisterSettings();
}

void FGoalOrientedActionPlanningEditor::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "GoalOrientedActionPlanning",
			LOCTEXT("RuntimeSettingsName", "Goal Oriented Action Planning"),
			LOCTEXT("RuntimeSettingsDescription", "Configure the Oriented Action Planning plugin"),
			GetMutableDefault<UGoapWorldStateRuntimeSettings>());
	}
}

void FGoalOrientedActionPlanningEditor::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "GoalOrientedActionPlanning");
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGoalOrientedActionPlanningEditor, GoalOrientedActionPlanningEditor);