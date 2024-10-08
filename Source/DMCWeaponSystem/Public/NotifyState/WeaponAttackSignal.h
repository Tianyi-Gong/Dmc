// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "WeaponAttackSignal.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DMCWEAPONSYSTEM_API UWeaponAttackSignal : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere)
	FGameplayTag IgnoreAttackSignalGamePlayTag;

	UPROPERTY(EditAnywhere)
	FGameplayTag AIAttackSignalGamePlayTag;

	UPROPERTY(EditAnywhere)
	float CheckAngle = 45;
};
