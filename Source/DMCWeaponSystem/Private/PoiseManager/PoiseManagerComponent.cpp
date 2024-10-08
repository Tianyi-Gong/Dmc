// Fill out your copyright notice in the Description page of Project Settings.


#include "PoiseManager/PoiseManagerComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemInterface.h"
#include "GoapAgentComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet\KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"

static float GLocalHitApplyForceScale = 200000;
static FAutoConsoleVariableRef CVarLocalHitApplyForceScale(
	TEXT("Weapon.LocalHitApplyForceScale"),
	GLocalHitApplyForceScale,
	TEXT("")
);

// Sets default values for this component's properties
UPoiseManagerComponent::UPoiseManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	PoiseAttributeSet = CreateDefaultSubobject<UPoiseAttributeSet>(TEXT("PoiseAttributeSet"));
	PoiseSystemModifyGameplayEffect = CreateDefaultSubobject<UPoiseSystemModifyGameplayEffect>(TEXT("PoiseSystemModifyGameplayEffect"));
}


// Called when the game starts
void UPoiseManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if(IAbilitySystemInterface* AsAbilitySystemInterface = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		OwnerAbilitySystemComponent = AsAbilitySystemInterface->GetAbilitySystemComponent();
		if( OwnerAbilitySystemComponent != nullptr)
		{
			OwnerAbilitySystemComponent->AddSpawnedAttribute(PoiseAttributeSet);

			if(PoiseRecoverEffet.Get() != nullptr)
			{
				OwnerAbilitySystemComponent->BP_ApplyGameplayEffectToSelf(PoiseRecoverEffet,1, FGameplayEffectContextHandle());
			}

			OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PoiseAttributeSet->GetPoiseAttribute()).AddUObject(this,&UPoiseManagerComponent::PoiseChanged);

			if(USkeletalMeshComponent* SkeletalMesh = GetOwner()->GetComponentByClass<USkeletalMeshComponent>())
			{
				if (UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance())
				{
					AnimInstance->OnMontageStarted.AddDynamic(this, &UPoiseManagerComponent::OnMontageStart);
					AnimInstance->OnMontageBlendingOut.AddDynamic(this, &UPoiseManagerComponent::OnMontageBlendOut);
				}
			}

			if(ActionPoiseCoonfigDataTable != nullptr)
			{
				TArray<FActionPoiseCoonfigDataTableRow*> Rows;
				ActionPoiseCoonfigDataTable->GetAllRows<FActionPoiseCoonfigDataTableRow>("",Rows);

				for (auto Row : Rows)
				{
					if(Row->ActionMontage == nullptr)
						continue;

					FString MontageName = Row->ActionMontage->GetName();

					const FActionPoiseCoonfigDataTableRow ActionPoiseCoonfigDataTableRow = *Row;

					ActionPoiseConfigDataMap.Add(MontageName, FActionPoiseConfigData(ActionPoiseCoonfigDataTableRow));
				}
			}

			if (LocalHitCurveFloat != nullptr)
			{
				OnLocalHitCurveFloatUpdateDelegate.BindDynamic(this, &UPoiseManagerComponent::OnLocalHitCurveFloatUpdate);
				OnLocalHitCurveFloatFinishDelegate.BindDynamic(this, &UPoiseManagerComponent::OnLocalHitCurveFloatFinish);

				LocalHitTime.AddInterpFloat(LocalHitCurveFloat, OnLocalHitCurveFloatUpdateDelegate);
				LocalHitTime.SetTimelineFinishedFunc(OnLocalHitCurveFloatFinishDelegate);
				LocalHitTime.SetTimelineLength(LocalHitCurveTimeLineLength);
			}

			if(BeatBackCurveFloat != nullptr)
			{
				OnBeatBackCurveFloatUpdateDelegate.BindDynamic(this, &UPoiseManagerComponent::OnBeatBackCurveFloatUpdate);
				OnBeatBackCurveFloatFinishDelegate.BindDynamic(this, &UPoiseManagerComponent::OnBeatBackCurveFloatFinish);

				BeatBackTimeLine.AddInterpFloat(BeatBackCurveFloat, OnBeatBackCurveFloatUpdateDelegate);
				BeatBackTimeLine.SetTimelineFinishedFunc(OnBeatBackCurveFloatFinishDelegate);
				BeatBackTimeLine.SetTimelineLength(BeatBackCurveTimeLineLength);
			}

			if(ACharacter* OwnerChar = Cast<ACharacter>(GetOwner()))
			{
				OwnerChar->MovementModeChangedDelegate.AddDynamic(this, &UPoiseManagerComponent::OnOwnerMoveModeChange);
			}
		}
	}
}


// Called every frame
void UPoiseManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (LocalHitCurveFloat != nullptr)
	{
		LocalHitTime.TickTimeline(DeltaTime);
	}

	if(BeatBackCurveFloat != nullptr)
	{
		BeatBackTimeLine.TickTimeline(DeltaTime);
	}
}

void UPoiseManagerComponent::OnMontageStart(UAnimMontage* Montage)
{
	FString MontageName = Montage->GetName();

	if(OwnerAbilitySystemComponent == nullptr)
		return;

	if(ActionPoiseConfigDataMap.Contains(MontageName))
	{
		PoiseSystemModifyGameplayEffect->MontageName = MontageName;
		PoiseSystemModifyGameplayEffect->bIsResetEffect = false;

		FActiveGameplayEffectHandle ActiveGameplayEffectHandle = OwnerAbilitySystemComponent->ApplyGameplayEffectToSelf(PoiseSystemModifyGameplayEffect,1, OwnerAbilitySystemComponent->MakeEffectContext());
	}
}

void UPoiseManagerComponent::OnMontageBlendOut(UAnimMontage* Montage, bool bInterrupted)
{
	FString MontageName = Montage->GetName();

	if (OwnerAbilitySystemComponent == nullptr)
		return;

	if (ActionPoiseConfigDataMap.Contains(MontageName))
	{
		PoiseSystemModifyGameplayEffect->MontageName = MontageName;
		PoiseSystemModifyGameplayEffect->bIsResetEffect = true;

		FActiveGameplayEffectHandle ActiveGameplayEffectHandle = OwnerAbilitySystemComponent->ApplyGameplayEffectToSelf(PoiseSystemModifyGameplayEffect, 1, OwnerAbilitySystemComponent->MakeEffectContext());
	}
}

void UPoiseManagerComponent::OnOwnerMoveModeChange(ACharacter* Character, EMovementMode PreMoveMode, uint8 PreviousCustomMode)
{
	if(PreMoveMode == EMovementMode::MOVE_Falling && Character->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
	{
		if (CheckAirFloatingStateGameplayTag.IsValid() &&
			AirFloatingEndGameplayTag.IsValid() &&
			OwnerAbilitySystemComponent->HasMatchingGameplayTag(CheckAirFloatingStateGameplayTag))
		{
			FGameplayEventData GameplayEventData;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), AirFloatingEndGameplayTag, GameplayEventData);
		}
	}
}

void UPoiseManagerComponent::GetLastHitMontageAnim(bool& Result, bool& IsSpecialAnim, UAnimMontage*& OutHitMontage, FSpecialHitAnimConfig& OutSpecialHitAnimConfig)
{
	Result = false;
	IsSpecialAnim = false;

	switch (HitActionImpactForce)
	{
	default:
		IsSpecialAnim = SpecialHitAnimMontageSet.Contains(HitActionImpactForce);
		if (IsSpecialAnim)
		{
			OutSpecialHitAnimConfig = SpecialHitAnimMontageSet.FindRef(HitActionImpactForce);
			break;
		}
		[[fallthrough]];
	case EActionImpactForce::ImpactForceLv_Large:
	case EActionImpactForce::ImpactForce_Max:
		Result = LargeHitAnimMontage.Contains(HitReactDirXY4Way);
		if (Result)
		{
			OutHitMontage = LargeHitAnimMontage.FindRef(HitReactDirXY4Way);
			break;
		}
		[[fallthrough]];
	case EActionImpactForce::ImpactForceLV_None:
	case EActionImpactForce::ImpactForceLV_Min:
	case EActionImpactForce::ImpactForceLv_Small:
	case EActionImpactForce::ImpactForceLv_Middle:
		Result = NormalHitAnimMontage.Contains(HitReactDirXY4Way);
		if (Result)
		{
			OutHitMontage = NormalHitAnimMontage.FindRef(HitReactDirXY4Way);
		}
		break;
	}
}

void UPoiseManagerComponent::PoiseChanged(const FOnAttributeChangeData& Data)
{
	if(Data.OldValue >= Data.NewValue)
	{
		if(Data.NewValue == 0 && PoiseRecoverDisableAfterFullEffect.Get() != nullptr)
		{
			//if(OwnerAbilitySystemComponent->GetGameplayEffectCount(PoiseRecoverDisableAfterFullEffect,nullptr) < 1)
			OwnerAbilitySystemComponent->BP_ApplyGameplayEffectToSelf(PoiseRecoverDisableAfterFullEffect,1, FGameplayEffectContextHandle());
		}
		else if(Data.OldValue > Data.NewValue && PoiseRecoverDisableEffect.Get() != nullptr)
		{
			OwnerAbilitySystemComponent->BP_ApplyGameplayEffectToSelf(PoiseRecoverDisableEffect, 1, FGameplayEffectContextHandle());
		}
	}

	if(APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if(UGoapAgentComponent* GoapAgent = OwnerPawn->GetController()->GetComponentByClass<UGoapAgentComponent>())
		{
			if(!GoapLowPoiseGameplayTag.IsValid())
				return;

			bool LowPoiseStateValue;
			if (!GoapAgent->GetGoapPrivateState(GoapLowPoiseGameplayTag, LowPoiseStateValue, true))
				return;

			if (Data.NewValue == 0 && !LowPoiseStateValue)
			{
				GoapAgent->SetGoapPrivateState(GoapLowPoiseGameplayTag, true);
			}
			else if(Data.NewValue > 0 && LowPoiseStateValue)
			{
				GoapAgent->SetGoapPrivateState(GoapLowPoiseGameplayTag, false);
			}
		}
	}
}

void UPoiseManagerComponent::OnLocalHitCurveFloatUpdate(float NewValue)
{
	if (LocalHitSkeletalMeshComponent != nullptr)
	{
		LocalHitSkeletalMeshComponent->SetAllBodiesBelowPhysicsBlendWeight(HitBoneName, NewValue, false, true);
		UE_VLOG_UELOG(this, LogTemp, Log, TEXT("NewValue is %f"), NewValue);
	}
}

void UPoiseManagerComponent::OnLocalHitCurveFloatFinish()
{
	if (LocalHitSkeletalMeshComponent != nullptr)
	{
		LocalHitSkeletalMeshComponent->SetAllBodiesPhysicsBlendWeight(0, false);
		LocalHitSkeletalMeshComponent->SetAllBodiesSimulatePhysics(false);
	}
}

void UPoiseManagerComponent::BeatBackStart(bool bIsBlockAttack)
{
	switch (HitActionImpactForce)
	{
	case EActionImpactForce::ImpactForceLV_None:
	case EActionImpactForce::ImpactForceLV_Min:
		BeatBackScale = 0;
		break;
	case EActionImpactForce::ImpactForceLv_Small:
	case EActionImpactForce::ImpactForceLv_Middle:
		BeatBackScale = bIsBlockAttack ? 0.5 : 0.25;
		break;
	case EActionImpactForce::ImpactForceLv_Large:
	case EActionImpactForce::ImpactForce_Max:
		BeatBackScale = 0;
		break;
	case EActionImpactForce::ImpactForce_UpperCut:
		BeatBackScale = 0;
		break;
	default:
		break;
	}

	if(BeatBackScale > 0)
	{
		BeatBackStartLocation = GetOwner()->GetActorLocation();
		BeatBackTimeLine.PlayFromStart();
	}
}

void UPoiseManagerComponent::OnBeatBackCurveFloatUpdate(float NewValue)
{
	FVector BeatBackLocation = BeatBackStartLocation + NewValue * BeatBackScale * BeatBackDirection;
	GetOwner()->SetActorLocation(BeatBackLocation,true);
}

void UPoiseManagerComponent::OnBeatBackCurveFloatFinish()
{

}

void UPoiseManagerComponent::UpdateHitReactDir(AActor* Attacker, const FHitResult& HitInfo, const FVector& HitDir, EActionImpactForce ActionImpactForce, bool bIsBlockAttack)
{
	FRotator OwnerActorRotator = GetOwner()->GetActorRotation();

	float HitReactAngle = UKismetAnimationLibrary::CalculateDirection(HitDir * FVector(1,1,0), OwnerActorRotator);

	const float AngleZone = 90;
	const float HalfAnfleZone = AngleZone / 2;

	float ClampAngle = FRotator::ClampAxis(HitReactAngle);
	float ConvertClampAngle = (ClampAngle >= 360 - HalfAnfleZone) ? HitReactAngle : ClampAngle;

	HitReactDirXY4Way = (EHitReactDirXY4Way)(FMath::RoundToInt(ClampAngle / AngleZone) % 4 + 1);

	HitActionImpactForce = ActionImpactForce;
	
	BeatBackDirection = (GetOwner()->GetActorLocation() - Attacker->GetActorLocation()).GetSafeNormal2D();

	if(HitActionImpactForce == EActionImpactForce::ImpactForce_UpperCut)
	{
		HitWorldDir = BeatBackDirection;
	}
	else
	{
		HitWorldDir = HitDir;
	}
	
	BeatBackStart(bIsBlockAttack);
}

void UPoiseManagerComponent::ReadyHitAnim(AActor* Attacker, const FHitResult& HitInfo, const FVector& HitDir, EActionImpactForce ActionImpactForce)
{
	APawn* HitPawn = Cast<APawn>(GetOwner());
	if (HitPawn == nullptr)
		return;

	AController* HitController = HitPawn->GetController();
	if (HitController == nullptr)
		return;

	UpdateHitReactDir(Attacker, HitInfo,HitDir, ActionImpactForce);

	UGoapAgentComponent* GoapAgent = HitController->GetComponentByClass<UGoapAgentComponent>();
	if (GoapAgent)
	{
		if (!GoapHitGameplayTag.IsValid())
			return;

		bool GoapPrivateStateValue = false;
		if(GoapAgent->GetGoapPrivateState(GoapHitGameplayTag, GoapPrivateStateValue, true))
		{
			if(!GoapPrivateStateValue)
			{
				GoapAgent->SetGoapPrivateState(GoapHitGameplayTag, true);
			}
			else
			{
				if(!HitGameplayEventTag.IsValid())
					return;

				FGameplayEventData GameplayEventData;
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), HitGameplayEventTag, GameplayEventData);
			}
		}		
	}
	else if (APlayerController* PlayerController = Cast<APlayerController>(HitController))
	{

	}
}

void UPoiseManagerComponent::TryDisEnableLocalHitEffet()
{
	if (LocalHitSkeletalMeshComponent != nullptr)
	{
		LocalHitTime.Stop();

		LocalHitSkeletalMeshComponent->SetAllBodiesPhysicsBlendWeight(0, false);
		LocalHitSkeletalMeshComponent->SetAllBodiesSimulatePhysics(false);

		LocalHitSkeletalMeshComponent = nullptr;
	}
}

bool UPoiseManagerComponent::TryEnableLocalHitEffet(AActor* Attacker, const FHitResult& HitInfo, const FVector& HitDir, EActionImpactForce ActionImpactForce)
{
	if (LocalHitCurveFloat == nullptr)
	{
		return false;
	}

	UpdateHitReactDir(Attacker, HitInfo, HitDir, ActionImpactForce, false);
	TryDisEnableLocalHitEffet();

	LocalHitSkeletalMeshComponent = Cast<USkeletalMeshComponent>(HitInfo.Component);

	if (LocalHitSkeletalMeshComponent != nullptr && LocalHitCurveFloat != nullptr)
	{
		if (HitInfo.BoneName == "None" || HitInfo.BoneName == "pelvis")
		{
			HitBoneName = "spine_02";
		}
		else
		{
			HitBoneName = HitInfo.BoneName;
		}

		float StartWeight = LocalHitCurveFloat->GetFloatValue(0);

		LocalHitSkeletalMeshComponent->SetAllBodiesBelowSimulatePhysics(HitBoneName, true, true);
		LocalHitSkeletalMeshComponent->SetAllBodiesBelowPhysicsBlendWeight(HitBoneName, StartWeight, false, true);

		FVector ApplayForce = HitDir * GLocalHitApplyForceScale;

		LocalHitSkeletalMeshComponent->AddForce(ApplayForce, HitBoneName, true);

		LocalHitTime.PlayFromStart();
	}

	return true;
}

void UPoiseManagerComponent::ReadyBlockAttackAnim(AActor* Attacker, const FHitResult& HitInfo, const FVector& HitDir, EActionImpactForce ActionImpactForce)
{
	if (!BlockAttackGameplayEventTag.IsValid())
		return;

	FGameplayEventData GameplayEventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), BlockAttackGameplayEventTag, GameplayEventData);

	UpdateHitReactDir(Attacker, HitInfo, HitDir, ActionImpactForce, true);
}

