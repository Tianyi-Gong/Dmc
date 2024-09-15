// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GoapAction.h"
#include "Animation/AnimMontage.h"
#include "GoapActionFaceToTarget.generated.h"

/**
 * 
 */
UCLASS(config = Game, BlueprintType, Blueprintable)
class GOALORIENTEDACTIONPLANNING_API UGoapActionFaceToTarget : public UGoapAction
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TArray<UAnimMontage*> TurnLAnimMontage;

	UPROPERTY(EditAnywhere)
	TArray<UAnimMontage*> TurnRAnimMontage;

	UPROPERTY(EditAnywhere)
	float PlayRate;

	UPROPERTY(EditAnywhere)
	FName TargetActorKey = "TargetActor";

	UAnimMontage* SelectTurnAnimMontage(float DelatPaw);

	virtual void OnActionActive() override;
};
