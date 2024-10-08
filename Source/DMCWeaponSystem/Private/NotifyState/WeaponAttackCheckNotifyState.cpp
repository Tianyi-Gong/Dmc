// Fill out your copyright notice in the Description page of Project Settings.


#include "NotifyState/WeaponAttackCheckNotifyState.h"
#include "WeaponMeshComponent.h"

void UWeaponAttackCheckNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	FindComponents = MeshComp->GetOwner()->K2_GetComponentsByClass(UWeaponMeshComponent::StaticClass());

	if(FindComponents.Num() == 0)
		return;

	for(auto ActorComponent : FindComponents)
	{
		UWeaponMeshComponent* WeaponMeshComponent = Cast<UWeaponMeshComponent>(ActorComponent);

		if(WeaponMeshComponent == nullptr)
			continue;

		WeaponMeshComponent->StartAttackCheck();
	}
}

void UWeaponAttackCheckNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (FindComponents.Num() == 0)
		return;

	for (auto ActorComponent : FindComponents)
	{
		UWeaponMeshComponent* WeaponMeshComponent = Cast<UWeaponMeshComponent>(ActorComponent);

		if (WeaponMeshComponent == nullptr)
			continue;

		WeaponMeshComponent->EndAttackCheck();
	}

	FindComponents.Empty();
}
