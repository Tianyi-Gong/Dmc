#include "GoapActions\GoapActionAbility.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"

UGoapActionAbility::~UGoapActionAbility()
{
    UnInitAbilityData();
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
