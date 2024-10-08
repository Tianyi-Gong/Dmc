#include "GameAbilitySystem/GoapAbilitySystemComponent.h"
#include "AbilitySystemLog.h"

bool FAbilityCDData::IsCoolDownComplete()
{
	return CurrentCoolDownTime <= 0;
}

bool FAbilityCDData::Active()
{
	if(IsCoolDownComplete())
	{
		CurrentCoolDownTime = CoolDownTime;

		return true;
	}

	return false;
}

int FAbilityCDData::GetCurrentWeight()
{
	return (!IsCoolDownComplete() && CoolDownTime > 0) ? 1 : Weight;
}

void FAbilityCDData::Tick(float DeltaTime)
{
	if(!IsCoolDownComplete())
	{
		CurrentCoolDownTime -= DeltaTime;
		if(IsCoolDownComplete())
		{
			CurrentCoolDownTime = 0;
		}
	}
}

FGameplayAbilitySpecHandle UGoapAbilitySystemComponent::GiveAbilityAndAddToAbilityGroup(const FAbilityCDConfig& AbilityCDConfig)
{
	if(AbilityCDConfig.PreloadedAbility == nullptr || !AbilityCDConfig.AbilityGroup.IsValid())
	{
		ABILITY_LOG(Error, TEXT("GiveAbility called with an invalid Ability Class."));

		return FGameplayAbilitySpecHandle();
	}

	FGameplayAbilitySpecHandle GameplayAbilitySpecHandle = GiveAbility(FGameplayAbilitySpec(AbilityCDConfig.PreloadedAbility, 1));
	
	if(GameplayAbilitySpecHandle.IsValid())
	{
		if(!AbilityGroupDataMap.Contains(AbilityCDConfig.AbilityGroup))
		{
			AbilityGroupDataMap.Add(AbilityCDConfig.AbilityGroup);
		}

		TArray<FAbilityCDData>* AbilityCDDataList = AbilityGroupDataMap.Find(AbilityCDConfig.AbilityGroup);

		FAbilityCDData AbilityCDData;
		AbilityCDData.CoolDownTime = AbilityCDConfig.CoolDownTime;
		AbilityCDData.GameplayAbilitySpecHandle = GameplayAbilitySpecHandle;
		AbilityCDData.Weight = AbilityCDConfig.Weight;
		AbilityCDData.PreloadedAbility = AbilityCDConfig.PreloadedAbility;

		AbilityCDDataList->Add(AbilityCDData);
	}

	UpdateShouldTick();

	return GameplayAbilitySpecHandle;
}

void UGoapAbilitySystemComponent::ClearAbilityAndRemoveFormAbilityGroup(const FAbilityCDConfig& AbilityCDConfig)
{
	if (AbilityCDConfig.PreloadedAbility == nullptr || !AbilityCDConfig.AbilityGroup.IsValid())
	{
		ABILITY_LOG(Error, TEXT("GiveAbility called with an invalid Ability Class."));
		return;
	}

	if(TArray<FAbilityCDData>* AbilityCDDataListPtr = AbilityGroupDataMap.Find(AbilityCDConfig.AbilityGroup))
	{
		int FindIndex = -1;
		for (int i=0; i < AbilityCDDataListPtr->Num(); i++)
		{
			const FAbilityCDData& AbilityCDData = AbilityCDDataListPtr->GetData()[i];
			if(AbilityCDData.PreloadedAbility == AbilityCDConfig.PreloadedAbility)
			{
				FindIndex = i;

				ClearAbility(AbilityCDData.GameplayAbilitySpecHandle);
			}
			break;
		}

		if(FindIndex != -1)
		{
			AbilityCDDataListPtr->RemoveAt(FindIndex);

			if(AbilityCDDataListPtr->Num() == 0)
			{
				AbilityGroupDataMap.Remove(AbilityCDConfig.AbilityGroup);
			}
		}
	}
}

FGameplayAbilitySpecHandle UGoapAbilitySystemComponent::TryActiveAbilityByWeightFromAbilityGroup(FGameplayTag AbilityGroup)
{
	FGameplayAbilitySpecHandle ActiveGameplayAbilitySpecHandle;

	if(!AbilityGroup.IsValid())
	{
		ABILITY_LOG(Error, TEXT("GiveAbility called with an invalid AbilityGroup."));
		return ActiveGameplayAbilitySpecHandle;
	}

	if(TArray<FAbilityCDData>* AbilityCDDataListPtr = AbilityGroupDataMap.Find(AbilityGroup))
	{
		int SumWeight = -1;

		for (auto& AbilityCDData : *AbilityCDDataListPtr)
		{
			SumWeight += AbilityCDData.GetCurrentWeight();
		}

		int RandValue = FMath::RandRange(0, SumWeight);

		SumWeight = 0;
		for (auto& AbilityCDData : *AbilityCDDataListPtr)
		{
			SumWeight += AbilityCDData.GetCurrentWeight();
			if(RandValue < SumWeight)
			{
				AbilityCDData.Active();
				ActiveGameplayAbilitySpecHandle = AbilityCDData.GameplayAbilitySpecHandle;
				break;
			}
		}

		TryActivateAbility(ActiveGameplayAbilitySpecHandle);
	}

	return ActiveGameplayAbilitySpecHandle;
}

bool UGoapAbilitySystemComponent::GetAbilityGroupInfo(FGameplayTag AbilityGroup, int& CDAbilityCount, int& AbilityCount)
{
	bool FindResult = false;
	if (TArray<FAbilityCDData>* AbilityCDDataListPtr = AbilityGroupDataMap.Find(AbilityGroup))
	{
		FindResult = true;
		AbilityCount = AbilityCDDataListPtr->Num();

		CDAbilityCount = 0;
		for (auto& AbilityCDData : *AbilityCDDataListPtr)
		{
			if(AbilityCDData.IsCoolDownComplete())
			{
				CDAbilityCount++;
			}
		}
	}
	else
	{
		CDAbilityCount = 0;
		AbilityCount = 0;
	}

	return FindResult;
}

bool UGoapAbilitySystemComponent::GetShouldTick() const
{
	bool ShouldTick = Super::GetShouldTick();

	if(!ShouldTick)
	{
		ShouldTick = AbilityGroupDataMap.Num() > 0;
	}

	return ShouldTick;
}

void UGoapAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (auto& [AbilityGroupDataKey, AbilityGroupDataValue] : AbilityGroupDataMap)
	{
		for (auto& AbilityCDData : AbilityGroupDataValue)
		{
			AbilityCDData.Tick(DeltaTime);
		}
	}
}
