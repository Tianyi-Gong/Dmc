// Fill out your copyright notice in the Description page of Project Settings.


#include "PoiseManager/GameplayEffectExecutionCalculation/PoiseSystemEffectExCalculation.h"
#include "PoiseManager/GameplayEffect/PoiseSystemModifyGameplayEffect.h"
#include "PoiseManager/PoiseManagerComponent.h"

struct PoiseSystemEffectStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(PoiseDamageCoefficient);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PoiseDefenceCoefficient);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ActionImpactForce);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorType);

	PoiseSystemEffectStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPoiseAttributeSet, PoiseDamageCoefficient, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPoiseAttributeSet, PoiseDefenceCoefficient, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPoiseAttributeSet, ActionImpactForce, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPoiseAttributeSet, ArmorType, Target, false);
	}
};

static const PoiseSystemEffectStatics& StaticsPoiseSystemEffect()
{
	static PoiseSystemEffectStatics PoiseSystemEffect;
	return PoiseSystemEffect;
}

UPoiseSystemEffectExCalculation::UPoiseSystemEffectExCalculation()
{
	RelevantAttributesToCapture.Add(StaticsPoiseSystemEffect().PoiseDamageCoefficientDef);
	RelevantAttributesToCapture.Add(StaticsPoiseSystemEffect().PoiseDefenceCoefficientDef);
	RelevantAttributesToCapture.Add(StaticsPoiseSystemEffect().ActionImpactForceDef);
	RelevantAttributesToCapture.Add(StaticsPoiseSystemEffect().ArmorTypeDef);
}

UPoiseSystemEffectExCalculation::~UPoiseSystemEffectExCalculation()
{
}

void UPoiseSystemEffectExCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	if(TargetAbilitySystemComponent == nullptr)
		return;

	UPoiseManagerComponent* PoiseManagerComponent = TargetAbilitySystemComponent->GetOwnerActor()->FindComponentByClass<UPoiseManagerComponent>();
	if(PoiseManagerComponent == nullptr)
		return;

	const UPoiseSystemModifyGameplayEffect* PoiseSystemModifyGameplayEffect = Cast<UPoiseSystemModifyGameplayEffect>(ExecutionParams.GetOwningSpecForPreExecuteMod()->Def.Get());
	if(PoiseSystemModifyGameplayEffect == nullptr)
		return;

	FString MontageName = PoiseSystemModifyGameplayEffect->MontageName;
	bool bIsResetEffect = PoiseSystemModifyGameplayEffect->bIsResetEffect;

	FActionPoiseConfigData* ActionPoiseConfigDataPtr = PoiseManagerComponent->ActionPoiseConfigDataMap.Find(MontageName);
	if(ActionPoiseConfigDataPtr == nullptr)
		return;

	float PoiseDamageModifyCoefficient = ActionPoiseConfigDataPtr->PoiseDamageModifyCoefficient * (bIsResetEffect ? -1 : 1);
	float PoiseDefenceModifyCoefficient = ActionPoiseConfigDataPtr->PoiseDefenceModifyCoefficient * (bIsResetEffect ? -1 : 1);
	float ActionImpactForce = (float)ActionPoiseConfigDataPtr->ActionImpactForce * (bIsResetEffect ? -1 : 1);
	float ArmorType = (float)ActionPoiseConfigDataPtr->ArmorTypeOffset * (bIsResetEffect ? -1 : 1);

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(StaticsPoiseSystemEffect().PoiseDamageCoefficientProperty, EGameplayModOp::Additive, PoiseDamageModifyCoefficient));
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(StaticsPoiseSystemEffect().PoiseDefenceCoefficientProperty, EGameplayModOp::Additive, PoiseDefenceModifyCoefficient));
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(StaticsPoiseSystemEffect().ActionImpactForceProperty, EGameplayModOp::Additive, ActionImpactForce));
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(StaticsPoiseSystemEffect().ArmorTypeProperty, EGameplayModOp::Additive, ArmorType));
}
