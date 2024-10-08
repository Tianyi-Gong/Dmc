// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoiseManager/PoiseAttributeSet.h"
#include "GameplayModMagnitudeCalculation.h"
#include "DisablePoiseRecoverCalculation.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class DMCWEAPONSYSTEM_API UDisablePoiseRecoverCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UDisablePoiseRecoverCalculation();

	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FProperty* PoiseRecoverProperty;
	FGameplayEffectAttributeCaptureDefinition PoiseRecoverDef;
};
