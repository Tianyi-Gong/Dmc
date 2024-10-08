// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoiseManager/PoiseAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "PoiseSystemEffectExCalculation.generated.h"

/**
 * 
 */
UCLASS()
class DMCWEAPONSYSTEM_API UPoiseSystemEffectExCalculation : public UGameplayEffectExecutionCalculation
{
	UPoiseSystemEffectExCalculation();
	~UPoiseSystemEffectExCalculation();
	
	GENERATED_BODY()
public:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
