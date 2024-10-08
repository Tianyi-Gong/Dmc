// Fill out your copyright notice in the Description page of Project Settings.


#include "PoiseManager/GameplayEffectExecutionCalculation/DisablePoiseRecoverCalculation.h"
#include "GameplayEffectAttributeCaptureDefinition.h"
#include "GameplayEffectTypes.h"



UDisablePoiseRecoverCalculation::UDisablePoiseRecoverCalculation()
{
	PoiseRecoverProperty = FindFieldChecked<FProperty>(UPoiseAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UPoiseAttributeSet, PoiseRecover));

	PoiseRecoverDef = FGameplayEffectAttributeCaptureDefinition(
		FGameplayAttribute(PoiseRecoverProperty),
		EGameplayEffectAttributeCaptureSource::Source,
		true);

	RelevantAttributesToCapture.Add(PoiseRecoverDef);
}

float UDisablePoiseRecoverCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;


	bool CaptureResult = false;

	float PoiseRecover = 0;

	CaptureResult = GetCapturedAttributeMagnitude(PoiseRecoverDef, Spec, EvaluationParameters, PoiseRecover);

	float PoiseRecoverDone = CaptureResult ? -PoiseRecover : 0;

	return PoiseRecoverDone;
}
