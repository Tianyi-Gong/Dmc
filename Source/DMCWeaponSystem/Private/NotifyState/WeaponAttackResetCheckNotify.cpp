// Fill out your copyright notice in the Description page of Project Settings.


#include "NotifyState/WeaponAttackResetCheckNotify.h"
#include "WeaponMeshComponent.h"

void UWeaponAttackResetCheckNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	TArray<UActorComponent*> FindComponents = MeshComp->GetOwner()->K2_GetComponentsByClass(UWeaponMeshComponent::StaticClass());

	if (FindComponents.Num() == 0)
		return;

	for (auto ActorComponent : FindComponents)
	{
		UWeaponMeshComponent* WeaponMeshComponent = Cast<UWeaponMeshComponent>(ActorComponent);

		if (WeaponMeshComponent == nullptr)
			continue;

		WeaponMeshComponent->RestAttackCheck();
	}
}
