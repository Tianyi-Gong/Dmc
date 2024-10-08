// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoiseManager/PoiseAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ResetPoiseEffectExCalculation.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class DMCWEAPONSYSTEM_API UResetPoiseEffectExCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
	UResetPoiseEffectExCalculation();
	~UResetPoiseEffectExCalculation();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
