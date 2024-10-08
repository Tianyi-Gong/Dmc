// Fill out your copyright notice in the Description page of Project Settings.

#include "DMCWeaponSystem.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FDMCWeaponSystem"

void FDMCWeaponSystem::StartupModule()
{
}

void FDMCWeaponSystem::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDMCWeaponSystem, DMCWeaponSystem);