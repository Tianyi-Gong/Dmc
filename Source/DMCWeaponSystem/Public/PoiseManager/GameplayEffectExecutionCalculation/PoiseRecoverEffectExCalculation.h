// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoiseManager/PoiseAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "PoiseRecoverEffectExCalculation.generated.h"
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class DMCWEAPONSYSTEM_API UPoiseRecoverEffectExCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UPoiseRecoverEffectExCalculation();
	~UPoiseRecoverEffectExCalculation();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};