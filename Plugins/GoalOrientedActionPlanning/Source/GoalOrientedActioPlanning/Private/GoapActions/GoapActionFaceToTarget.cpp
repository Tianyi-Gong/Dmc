// Fill out your copyright notice in the Description page of Project Settings.

#include "GoapActions/GoapActionFaceToTarget.h"
#include "GameFramework/Character.h"
#include "Runtime\AIModule\Classes\BehaviorTree\BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionWarpingComponent.h"

UAnimMontage* UGoapActionFaceToTarget::SelectTurnAnimMontage(float DelatPaw)
{
	if(TurnLAnimMontage.Num() != TurnRAnimMontage.Num() || TurnRAnimMontage.Num() == 0)
	{
		return nullptr;
	}
	float SlecteTurnPaw = TurnRAnimMontage.Num() > 1 ? 120 : 180;

	int SelectIndex = FMath::Abs(DelatPaw) <= SlecteTurnPaw ? 0 : 1;

	if(DelatPaw > 0)
	{
		return TurnRAnimMontage[SelectIndex];
	}
	else
	{
		return TurnLAnimMontage[SelectIndex];
	}
}

void UGoapActionFaceToTarget::OnActionActive()
{
	Super::OnActionActive();

	ACharacter* ControlledCharacter = Cast<ACharacter>(ControlledPawn);
	if(ControlledCharacter == nullptr || AIController == nullptr)
		EndAction(EGoapActionResult::Failed);

	UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();

	if(Blackboard == nullptr)
		EndAction(EGoapActionResult::Failed);

	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey));
	if(TargetActor == nullptr)
		EndAction(EGoapActionResult::Failed);

	FVector ControllerCharLocation2D = ControlledCharacter->GetActorLocation() * FVector(1,1,0);
	FVector TargetActorLocation2D = TargetActor->GetActorLocation() * FVector(1, 1, 0);

	FRotator LockRotator = UKismetMathLibrary::FindLookAtRotation(ControllerCharLocation2D, TargetActorLocation2D);
	FRotator ControllerCharRotator = ControlledCharacter->GetActorRotation();

	float DeltaAngle = FMath::FindDeltaAngleDegrees(ControllerCharRotator.Yaw, LockRotator.Yaw);
	
	UAnimMontage* AnimMontage = SelectTurnAnimMontage(DeltaAngle);

	if(AnimMontage == nullptr)
		EndAction(EGoapActionResult::Failed);

	UAnimInstance* AnimInstance = ControlledCharacter->GetMesh()->GetAnimInstance();
	if(AnimInstance == nullptr)
		EndAction(EGoapActionResult::Failed);

	if (UMotionWarpingComponent* MotionWarpingComponent = ControlledPawn->GetComponentByClass<UMotionWarpingComponent>())
	{
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(MotionWarpingTag, FVector::ZeroVector, LockRotator);
	}

	AnimInstance->Montage_Play(AnimMontage,PlayRate);

	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
	{
		EndAction( bInterrupted ? EGoapActionResult::Failed : EGoapActionResult::Succeeded );
	});

	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, AnimMontage);
}
