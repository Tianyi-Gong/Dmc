// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify\AIComboAttackCheckAnimNotify.h"
#include "Kismet\KismetSystemLibrary.h"
#include "Kismet\KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "Runtime\AIModule\Classes\AIController.h"
#include "Runtime\AIModule\Classes\BehaviorTree\BlackboardComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "MotionWarpingComponent.h"

static int GAIComboAttackCheckDebugType = 0;
static FAutoConsoleVariableRef CVarAIComboAttackCheckDebugType(
	TEXT("AIDebug.AIComboAttackCheckDebugType"),
	GAIComboAttackCheckDebugType,
	TEXT(" 0=None 1=ForOneFrame 2=ForDuration 3=Persistent")
	);

static bool GAIComboAttackForceWarping = false;
static FAutoConsoleVariableRef CVarAIComboAttackForceWarping(
	TEXT("AIDebug.AIComboAttackForceWarping"),
	GAIComboAttackForceWarping,
	TEXT(" 0=Disable 1=Enable")
);

void UAIComboAttackCheckAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp,Animation,EventReference);

	if(!EventTag.IsValid())
		return;

	ACharacter* TraceActor = Cast<ACharacter>(MeshComp->GetOwner());
	if(TraceActor == nullptr || TraceActor->GetController() == nullptr)
		return;

	AAIController* AIController = Cast<AAIController>(TraceActor->GetController());

	if(AIController == nullptr)
		return;

	UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();

	if(Blackboard == nullptr)
		return;

	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetActorBlackBoardkey));
	if(TargetActor == nullptr)
		return;

	bool CheckResult = false;

	if(GAIComboAttackForceWarping == false)
	{
		switch (AIComboAttackType)
		{
			case EAIComboAttackType::SphereTrace:
			{
				FVector TraceStart = TraceActor->GetActorLocation() +
					TraceActor->GetActorForwardVector() * AIForwardTraceDistance +
					TraceActor->GetActorUpVector() * -AIAISphereTraceHalfHeight;

				FVector TraceEnd = TraceStart + TraceActor->GetActorUpVector() * AIAISphereTraceHalfHeight * 2;

				FHitResult HitResult;
				bool bFindResult = UKismetSystemLibrary::SphereTraceSingleForObjects(TraceActor, TraceStart, TraceEnd, AISphereTraceRadiu, { EObjectTypeQuery::ObjectTypeQuery3 }, false, {}, (EDrawDebugTrace::Type)GAIComboAttackCheckDebugType, HitResult, true);

				if (bFindResult && HitResult.GetActor() == TargetActor)
				{
					CheckResult = true;
				}
			}
			break;
			case EAIComboAttackType::Chance:
			{
				int randValue = FMath::RandHelper(99);
				if(Chance > randValue)
				{
					CheckResult = true;
				}
			}
			break;

			case EAIComboAttackType::AngleCheck:
			default:
			{
				FVector TraceActorLocation = TraceActor->GetActorLocation();
				FVector TargetActorLocation = TargetActor->GetActorLocation();
				float CosAngle2D = (TargetActorLocation - TraceActorLocation).CosineAngle2D(TraceActor->GetActorForwardVector());
				if(CosAngle2D >= UKismetMathLibrary::DegCos(AngleDifference))
				{
					CheckResult = true;
				}
			}
			break;
		}
	}
	else
	{
		CheckResult = true;
	}
	
	if(CheckResult)
	{
		FGameplayEventData GameplayEventData;
		GameplayEventData.EventTag = EventTag;
		GameplayEventData.Target = TargetActor;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TraceActor,EventTag, GameplayEventData);

		if(UMotionWarpingComponent* MotionWarpingComponent = TraceActor->GetComponentByClass<UMotionWarpingComponent>())
		{
			FVector LookAtStart = TraceActor->GetActorLocation() * FVector(1,1,0);
			FVector LookAtEnd = TargetActor->GetActorLocation() * FVector(1,1,0);

			MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(MotionWarpingTag,FVector::ZeroVector, UKismetMathLibrary::FindLookAtRotation(LookAtStart, LookAtEnd));
		}
	}
}
