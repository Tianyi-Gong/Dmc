// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/ApplyGameplayEffectNotifyState.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"


void UApplyGameplayEffectNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(MeshComp->GetOwner());
	if(AbilitySystemInterface == nullptr)
		return;

	UAbilitySystemComponent* AbilitySystem = AbilitySystemInterface->GetAbilitySystemComponent();

	if(AbilitySystem == nullptr || GameplayEffectClass.Get() == nullptr)
		return;

	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = AbilitySystem->BP_ApplyGameplayEffectToSelf(GameplayEffectClass,1, FGameplayEffectContextHandle());

}

void UApplyGameplayEffectNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(MeshComp->GetOwner());
	if (AbilitySystemInterface == nullptr)
		return;

	UAbilitySystemComponent* AbilitySystem = AbilitySystemInterface->GetAbilitySystemComponent();

	if (AbilitySystem == nullptr || GameplayEffectClass.Get() == nullptr)
		return;

	AbilitySystem->RemoveActiveGameplayEffectBySourceEffect(GameplayEffectClass, AbilitySystem);
}
