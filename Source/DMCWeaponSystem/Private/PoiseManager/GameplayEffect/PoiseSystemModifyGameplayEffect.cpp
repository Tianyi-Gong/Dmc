// Fill out your copyright notice in the Description page of Project Settings.


#include "PoiseManager/GameplayEffect/PoiseSystemModifyGameplayEffect.h"
#include "PoiseManager/GameplayEffectExecutionCalculation/PoiseSystemEffectExCalculation.h"

UPoiseSystemModifyGameplayEffect::UPoiseSystemModifyGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayEffectExecutionDefinition GameplayEffectExecutionDefinition = FGameplayEffectExecutionDefinition();
	GameplayEffectExecutionDefinition.CalculationClass = UPoiseSystemEffectExCalculation::StaticClass();
	Executions.Add(GameplayEffectExecutionDefinition);
}
