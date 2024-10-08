// Fill out your copyright notice in the Description page of Project Settings.


#include "PoiseManager/GameplayEffectExecutionCalculation/PoiseRecoverEffectExCalculation.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

struct PoiseRecoverStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(PoiseRecover);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Poise);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxPoise);

	PoiseRecoverStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPoiseAttributeSet, PoiseRecover, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPoiseAttributeSet, Poise, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPoiseAttributeSet, MaxPoise, Target, false);
	}
};

static const PoiseRecoverStatics& StaticsPoiseRecover()
{
	static PoiseRecoverStatics PoiseRecover;
	return PoiseRecover;
}

UPoiseRecoverEffectExCalculation::UPoiseRecoverEffectExCalculation()
{
	RelevantAttributesToCapture.Add(StaticsPoiseRecover().PoiseDef);
	RelevantAttributesToCapture.Add(StaticsPoiseRecover().MaxPoiseDef);
	RelevantAttributesToCapture.Add(StaticsPoiseRecover().PoiseRecoverDef);
}

UPoiseRecoverEffectExCalculation::~UPoiseRecoverEffectExCalculation()
{
}

void UPoiseRecoverEffectExCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
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

	float Poise = 0.0f;
	CaptureResult = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(StaticsPoiseRecover().PoiseDef, EvaluationParameters, Poise);

	if (!CaptureResult)
		return;

	float MaxPoise = 0.0f;
	CaptureResult = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(StaticsPoiseRecover().MaxPoiseDef, EvaluationParameters, MaxPoise);

	if (Poise == MaxPoise || !CaptureResult)
	{
		return;
	}

	float PoiseRecover = 0.0f;
	CaptureResult = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(StaticsPoiseRecover().PoiseRecoverDef, EvaluationParameters, PoiseRecover);

	if (PoiseRecover == 0 || !CaptureResult)
	{
		return;
	}

	const FGameplayEffectSpec* GameplayEffectSpec = ExecutionParams.GetOwningSpecForPreExecuteMod();
	float Period = GameplayEffectSpec->GetPeriod();

	float PoiseDone = Poise + PoiseRecover * Period;
	PoiseDone = FMath::Clamp(PoiseDone, 0, MaxPoise);

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(StaticsPoiseRecover().PoiseProperty, EGameplayModOp::Override, PoiseDone));
}
