// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoiseManager/PoiseAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "PoiseDamageEffectExCalculation.generated.h"

/**
 * 
 */
UCLASS()
class DMCWEAPONSYSTEM_API UPoiseDamageEffectExCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UPoiseDamageEffectExCalculation();
	~UPoiseDamageEffectExCalculation();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
