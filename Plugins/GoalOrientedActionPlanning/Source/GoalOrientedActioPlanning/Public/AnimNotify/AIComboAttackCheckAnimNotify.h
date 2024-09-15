// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AIComboAttackCheckAnimNotify.generated.h"

UENUM(BlueprintType)
enum class EAIComboAttackType : uint8
{
	AngleCheck,
	SphereTrace
};
/**
 * 
 */
UCLASS(Blueprintable)
class GOALORIENTEDACTIONPLANNING_API UAIComboAttackCheckAnimNotify : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	EAIComboAttackType AIComboAttackType;

	UPROPERTY(EditInstanceOnly,BlueprintReadWrite, meta = (EditCondition = "AIComboAttackType == EAIComboAttackType::SphereTrace", EditConditionHides))
	float AIForwardTraceDistance = 125;

	UPROPERTY(EditInstanceOnly,BlueprintReadWrite, meta = (EditCondition = "AIComboAttackType == EAIComboAttackType::SphereTrace", EditConditionHides))
	float AISphereTraceRadiu = 50;

	UPROPERTY(EditInstanceOnly,BlueprintReadWrite, meta = (EditCondition = "AIComboAttackType == EAIComboAttackType::SphereTrace", EditConditionHides))
	float AIAISphereTraceHalfHeight = 40;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (EditCondition = "AIComboAttackType == EAIComboAttackType::AngleCheck", EditConditionHides))
	float AngleDifference = 90;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	FName TargetActorBlackBoardkey = "TargetActor";

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	FGameplayTag EventTag;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	FName MotionWarpingTag;
};
