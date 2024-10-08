// Fill out your copyright notice in the Description page of Project Settings.


#include "PoiseManager/GameplayEffectExecutionCalculation/ResetPoiseEffectExCalculation.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

struct RestPoiseStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Poise);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxPoise);

	RestPoiseStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPoiseAttributeSet, Poise, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPoiseAttributeSet, MaxPoise, Target, false);
	}
};

static const RestPoiseStatics& StaticsRestPoise()
{
	static RestPoiseStatics RestPoise;
	return RestPoise;
}

UResetPoiseEffectExCalculation::UResetPoiseEffectExCalculation()
{
	RelevantAttributesToCapture.Add(StaticsRestPoise().PoiseDef);
	RelevantAttributesToCapture.Add(StaticsRestPoise().MaxPoiseDef);
}

UResetPoiseEffectExCalculation::~UResetPoiseEffectExCalculation()
{
}

void UResetPoiseEffectExCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
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
	CaptureResult = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(StaticsRestPoise().PoiseDef, EvaluationParameters, Poise);

	if (!CaptureResult)
		return;

	float MaxPoise = 0.0f;
	CaptureResult = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(StaticsRestPoise().MaxPoiseDef, EvaluationParameters, MaxPoise);

	if (Poise == MaxPoise || !CaptureResult)
	{
		return;
	}

	float PoiseDone = MaxPoise;

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(StaticsRestPoise().PoiseProperty, EGameplayModOp::Override, PoiseDone));
}
