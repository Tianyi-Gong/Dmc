// Fill out your copyright notice in the Description page of Project Settings.


#include "NotifyState/WeaponAttackSignal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Kismet\KismetSystemLibrary.h"
#include "Kismet\KismetMathLibrary.h"
#include "Runtime\AIModule\Classes\AIController.h"
#include "GoapAgentComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

static int GAIAttackSignalCheckDebugType = 0;
static FAutoConsoleVariableRef CVarAIComboAttackCheckDebugType(
	TEXT("AIDebug.AIAttackSignalCheckDebugType"),
	GAIAttackSignalCheckDebugType,
	TEXT(" 0=None 1=ForOneFrame 2=ForDuration 3=Persistent")
);

void UWeaponAttackSignal::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ACharacter* TraceCharacter = Cast<ACharacter>(MeshComp->GetOwner());
	if (TraceCharacter == nullptr || TraceCharacter->GetController() == nullptr)
		return;

	float ScaledCapsuleHalfHeight = TraceCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	float ScaledCapsuleRadius = TraceCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();

	FVector TraceStart = TraceCharacter->GetActorLocation() +
						 TraceCharacter->GetActorForwardVector() * ScaledCapsuleRadius * 2;

	FVector TraceEnd = TraceStart + TraceCharacter->GetActorForwardVector() * ScaledCapsuleRadius * 6;

	FHitResult HitResult;
	bool CheckResult = UKismetSystemLibrary::CapsuleTraceSingleForObjects(TraceCharacter,TraceStart,TraceEnd, ScaledCapsuleRadius, ScaledCapsuleHalfHeight, { EObjectTypeQuery::ObjectTypeQuery3 },false,{}, (EDrawDebugTrace::Type)GAIAttackSignalCheckDebugType, HitResult, true);
	if(!CheckResult)
		return;

	ACharacter* AIAgent = Cast<ACharacter>(HitResult.GetActor());
	if(AIAgent == nullptr)
		return;

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(AIAgent);
	if(AbilitySystemInterface == nullptr)
		return;

	UAbilitySystemComponent* AgentAbilitySystem = AbilitySystemInterface->GetAbilitySystemComponent();

	AAIController* AIController = Cast<AAIController>(AIAgent->GetController());
	if (AIController == nullptr)
		return;

	UGoapAgentComponent* GoapAgent = AIController->GetComponentByClass<UGoapAgentComponent>();
	if(	GoapAgent != nullptr && 
		IgnoreAttackSignalGamePlayTag.IsValid() &&
		AIAttackSignalGamePlayTag.IsValid() &&
		!AgentAbilitySystem->HasMatchingGameplayTag(IgnoreAttackSignalGamePlayTag) )
	{
		FVector AgentForwardVector2D = -AIAgent->GetActorForwardVector();
		FVector TraceCharacterForwardVector2D = TraceCharacter->GetActorForwardVector();

		if (AgentForwardVector2D.CosineAngle2D(TraceCharacterForwardVector2D) >= UKismetMathLibrary::DegCos(CheckAngle))
		{
			GoapAgent->SetGoapPrivateState(AIAttackSignalGamePlayTag, true);
		}
	}
}