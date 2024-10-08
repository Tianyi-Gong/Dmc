// Fill out your copyright notice in the Description page of Project Settings.


#include "PoiseManager/GameplayEffectExecutionCalculation/PoiseDamageEffectExCalculation.h"

struct PoiseDamageEffectStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Poise);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PoiseDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PoiseDamageCoefficient);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PoiseDefenceCoefficient);

	PoiseDamageEffectStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPoiseAttributeSet, Poise, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPoiseAttributeSet, PoiseDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPoiseAttributeSet, PoiseDamageCoefficient, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPoiseAttributeSet, PoiseDefenceCoefficient, Target, false);
	}
};

static const PoiseDamageEffectStatics& StaticsPoiseDamageEffect()
{
	static PoiseDamageEffectStatics PoiseDamageEffect;
	return PoiseDamageEffect;
}

UPoiseDamageEffectExCalculation::UPoiseDamageEffectExCalculation()
{
	RelevantAttributesToCapture.Add(StaticsPoiseDamageEffect().PoiseDef);
	RelevantAttributesToCapture.Add(StaticsPoiseDamageEffect().PoiseDamageDef);
	RelevantAttributesToCapture.Add(StaticsPoiseDamageEffect().PoiseDamageCoefficientDef);
	RelevantAttributesToCapture.Add(StaticsPoiseDamageEffect().PoiseDefenceCoefficientDef);
}

UPoiseDamageEffectExCalculation::~UPoiseDamageEffectExCalculation()
{
}

void UPoiseDamageEffectExCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	bool CaptureResult = false;

	float PoiseDamage = 0.0f;
	CaptureResult = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(StaticsPoiseDamageEffect().PoiseDamageDef, EvaluationParameters, PoiseDamage);
	if(!CaptureResult)
		return;

	float PoiseDamageCoefficient = 0.0f;
	CaptureResult = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(StaticsPoiseDamageEffect().PoiseDamageCoefficientDef, EvaluationParameters, PoiseDamageCoefficient);
	if(!CaptureResult)
		return;

	float PoiseDefenceCoefficient = 0.0f;
	CaptureResult = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(StaticsPoiseDamageEffect().PoiseDefenceCoefficientDef, EvaluationParameters, PoiseDefenceCoefficient);
	if(!CaptureResult)
		return;

	float Poise = 0.0f;
	CaptureResult = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(StaticsPoiseDamageEffect().PoiseDef, EvaluationParameters, Poise);
	if (!CaptureResult)
		return;

	float PoiseDamageDone = PoiseDamage * PoiseDamageCoefficient * (1 - PoiseDefenceCoefficient);

	if(Poise > 0 && PoiseDamageDone > 0)
	{
		float PoiseDone = FMath::Clamp(Poise - PoiseDamageDone,0, Poise);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(StaticsPoiseDamageEffect().PoiseProperty, EGameplayModOp::Override, PoiseDone));
	}
}
