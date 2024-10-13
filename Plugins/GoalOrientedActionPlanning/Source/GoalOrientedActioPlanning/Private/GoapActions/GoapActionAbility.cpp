#include "GoapActions\GoapActionAbility.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionWarpingComponent.h"
#include "Kismet/KismetSystemLibrary.h"

static bool GAttackWarpingDebug = false;
static FAutoConsoleVariableRef CVarAIComboAttackCheckDebugType(
    TEXT("Weapon.AttackWarpingDebug"),
    GAttackWarpingDebug,
    TEXT(" 0=Disable 1=Enable")
);

UGoapActionAbility::~UGoapActionAbility()
{
    UnInitAbilityData();
}

void UGoapActionAbility::MotionWarpingToTarget(FName MotionWarpingTag, FName TargetActorKey, bool bWarpingLocation, float PredictTargetLocationTime, float PredictTargetLocationOffset)
{
    if(MotionWarpingTag == "None" || TargetActorKey == "None" || ControlledPawn == nullptr)
        return;

    if (UMotionWarpingComponent* MotionWarpingComponent = ControlledPawn->GetComponentByClass<UMotionWarpingComponent>())
    {
        if (AIController == nullptr)
           return;

        UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();

        if (Blackboard == nullptr)
           return;

        AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey));
        if (TargetActor == nullptr)
            return;

        FVector ControllerCharLocation2D = ControlledPawn->GetActorLocation() * FVector(1, 1, 0);
        FVector TargetActorLocation2D = TargetActor->GetActorLocation() * FVector(1, 1, 0);

        FVector LockLocation;
        if(bWarpingLocation)
        {
            FVector TargetActorPredictTargetLocation2D = TargetActor->GetActorLocation() + (TargetActor->GetVelocity() * PredictTargetLocationTime * FVector(1,1,0));
            LockLocation = TargetActorPredictTargetLocation2D - (ControlledPawn->GetActorLocation() - TargetActorPredictTargetLocation2D).GetSafeNormal2D() * PredictTargetLocationOffset;
            TargetActorLocation2D = LockLocation * FVector(1,1,0);
           

            if(GAttackWarpingDebug)
            {
                UKismetSystemLibrary::DrawDebugSphere(ControlledPawn, TargetActorPredictTargetLocation2D, 16, 12, FLinearColor::Yellow, 5);
                UKismetSystemLibrary::DrawDebugSphere(ControlledPawn, LockLocation, 16, 12, FLinearColor::Green, 5);
            }
        }

        FRotator LockRotator = UKismetMathLibrary::FindLookAtRotation(ControllerCharLocation2D, TargetActorLocation2D);
        FRotator ControllerCharRotator = ControlledPawn->GetActorRotation();

        float DeltaAngle = FMath::FindDeltaAngleDegrees(ControllerCharRotator.Yaw, LockRotator.Yaw);

        MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(MotionWarpingTag, LockLocation, LockRotator);
    }
}

void UGoapActionAbility::ActiveAbilityByAbilityGroupTag(FGameplayTag GameplayTag, bool& bResult, FGameplayAbilitySpecHandle& GameplayAbilitySpecHandle)
{
    if(GoapAbilitySystemComponent == nullptr)
    {
        bResult = false;
        return;
    }

    ActiveGameplayAbilitySpecHandle = GoapAbilitySystemComponent->TryActiveAbilityByWeightFromAbilityGroup(GameplayTag);

    if(ActiveGameplayAbilitySpecHandle.IsValid())
    {
        bResult = true;
        GameplayAbilitySpecHandle = ActiveGameplayAbilitySpecHandle;
    }
    else
    {
        bResult = false;
    }
}

void UGoapActionAbility::InitControlerData(AAIController* InAIController)
{
	Super::InitControlerData(InAIController);

    InitAbilityData();
}

void UGoapActionAbility::InitAbilityData()
{
    if (ControlledPawn)
    {
        IAbilitySystemInterface* AsAbilitySysInterface = Cast<IAbilitySystemInterface>(ControlledPawn);
        if(AsAbilitySysInterface == nullptr)
            return;

        if (UAbilitySystemComponent* AbilitySystemComponent = AsAbilitySysInterface->GetAbilitySystemComponent())
        {
            GoapAbilitySystemComponent = Cast<UGoapAbilitySystemComponent>(AbilitySystemComponent);
            if (GoapAbilitySystemComponent == nullptr)
                return;

            for (auto& AbilityConfig : AbilityConfigList)
            {
                if (AbilityConfig.PreloadedAbility != nullptr)
                {
                    GoapAbilitySystemComponent->GiveAbilityAndAddToAbilityGroup(AbilityConfig);
                }
            }

            AbilityEndDelegateHandle = GoapAbilitySystemComponent->OnAbilityEnded.AddUObject(this, &UGoapActionAbility::OnAbilityEndDelegate);
        }
    }
}

void UGoapActionAbility::UnInitAbilityData()
{
    if(ControlledPawn && GoapAbilitySystemComponent)
    {
        for (auto& AbilityConfig : AbilityConfigList)
        {
            if (AbilityConfig.PreloadedAbility != nullptr)
            {
                GoapAbilitySystemComponent->ClearAbilityAndRemoveFormAbilityGroup(AbilityConfig);
            }
        }

        GoapAbilitySystemComponent->OnAbilityEnded.Remove(AbilityEndDelegateHandle);
    }
}

void UGoapActionAbility::OnAbilityEndDelegate(const FAbilityEndedData& AbilityEndedData)
{
    if(!ActiveGameplayAbilitySpecHandle.IsValid())
        return;

    if(AbilityEndedData.AbilitySpecHandle == ActiveGameplayAbilitySpecHandle)
    {
        ReciveOnAbilityEndDelegate(ActiveGameplayAbilitySpecHandle, AbilityEndedData.bWasCancelled);
    }
}

void UGoapActionAbility::UpdateDynamicCostByGoapState(const FGoapWorldState& GoapWorldState)
{
    Super::UpdateDynamicCostByGoapState(GoapWorldState);

    if(bUpdateCostByAbilityGroupCDInfo && CheckAbilityGroupGameplayTag.IsValid())
    {
        int CDAbilityCount = 0;
        int AbilityCount = 0;
        if(GoapAbilitySystemComponent->GetAbilityGroupInfo(CheckAbilityGroupGameplayTag, CDAbilityCount, AbilityCount))
        {
            DynamicCost = CDAbilityCount;
        }
    }
}
