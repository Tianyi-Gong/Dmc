#include "GoapGoalSet.h"
#include "Engine/AssetManager.h"

void UGoapGoalSet::InitData(AAIController* Controller)
{
	AIController = Controller;

	TArray<FSoftObjectPath> LoadPath;

	for (auto GoapGoalConfigGroupData : GoapGoalConfig)
	{
		for (auto GoapGoalConfigData : GoapGoalConfigGroupData.GoapGoalConfigDataList)
		{
			LoadPath.Add(FSoftObjectPath(GoapGoalConfigData.GoapGoalClass->GetClassPathName()));
		}
	}

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	Streamable.RequestAsyncLoad(LoadPath, FStreamableDelegate::CreateLambda( 
		[this]()
		{
			GoapGoalPool.Empty();

			for (auto GoapGoalConfigGroupData : GoapGoalConfig)
			{
				FGoapGoalSubGroup GoapGoalSubGroup;
				GoapGoalSubGroup.bRandBeforSelectGoal = GoapGoalConfigGroupData.bRandBeforSelectGoal;
				GoapGoalSubGroup.bRandByWeight = GoapGoalConfigGroupData.bRandByWeight;

				for (auto GoapGoalConfigData : GoapGoalConfigGroupData.GoapGoalConfigDataList)
				{
					UGoapGoal* GoapGoal = NewObject<UGoapGoal>(GetOuter(), GoapGoalConfigData.GoapGoalClass);

					if (GoapGoal)
					{
						check(GoapGoal->GoapWorldStateRuntimeSettingClass == GoapWorldStateRuntimeSettingClass)
							if (GoapGoal->GoapWorldStateRuntimeSettingClass == GoapWorldStateRuntimeSettingClass)
							{
								FGoapGoalSubGroupData GoapGoalSubGroupData;
								GoapGoalSubGroupData.GoapGoalData = GoapGoal;
								GoapGoalSubGroupData.Weight = GoapGoalConfigData.Weight > 0 ? GoapGoalConfigData.Weight : 1;

								GoapGoal->InitGoapState();
								GoapGoal->InitControlerData(AIController);
								GoapGoalSubGroup.GoapGoalSupGroup.Add(GoapGoalSubGroupData);
							}
					}
				}
				GoapGoalPool.Add(GoapGoalSubGroup);
			}

			bIsDataLoaded = true;
		}));
}

UGoapGoal* UGoapGoalSet::SelectGoalByWorldState(const FGoapWorldState& GoapWorldState)
{
	for (auto GoapSubGroup : GoapGoalPool)
	{
		GoapSubGroup.ShuffleGroup();
		for(auto GoapGoalSubGroupData : GoapSubGroup.GoapGoalSupGroup)
		{
			UGoapGoal* GoapGoal = GoapGoalSubGroupData.GoapGoalData;
			int64 CareFlag = ~GoapGoal->Precondition.NotUsedFlag;
			if ((GoapGoal->Precondition.Values & CareFlag) == (GoapWorldState.Values & CareFlag))
			{
				GoapGoal->SetGoapGoalResult(EGoapGoalResult::InProgress);
				return GoapGoal;
			}
		}
	}

	return nullptr;
}

#if WITH_EDITOR
void UGoapGoalSet::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property != nullptr)
	{
		const FName PropertyName(PropertyChangedEvent.Property->GetFName());
		if (PropertyName == GET_MEMBER_NAME_CHECKED(FGoapGoalConfigGroupData, Index))
		{
			for (auto& GoapGoalConfigGroupData : GoapGoalConfig)
			{
				if(GoapGoalConfigGroupData.Index * 2 != GoapGoalConfigGroupData.Priority)
				{
					GoapGoalConfigGroupData.Priority = (GoapGoalConfigGroupData.Index - 1) * 2 + 1;
				}
			}

			GoapGoalConfig.Sort(
				[](const FGoapGoalConfigGroupData& A, const FGoapGoalConfigGroupData& B)
				{
					return A.Priority < B.Priority;
				});

			for (int i = 0; i < GoapGoalConfig.Num(); i++)
			{
				GoapGoalConfig[i].Index = (i + 1);
				GoapGoalConfig[i].Priority = (i + 1) * 2;
			}
		}
		else if(PropertyName == GET_MEMBER_NAME_CHECKED(UGoapGoalSet, GoapGoalConfig))
		{
			for (int i = 0; i < GoapGoalConfig.Num(); i++)
			{
				GoapGoalConfig[i].Index = (i + 1);
				GoapGoalConfig[i].Priority = (i + 1) * 2;
			}

			GoapGoalConfig.Sort(
				[](const FGoapGoalConfigGroupData& A, const FGoapGoalConfigGroupData& B)
				{
					return A.Priority < B.Priority;
				});
		}
		else if(PropertyName == GET_MEMBER_NAME_CHECKED(FGoapGoalConfigGroupData, bRandBeforSelectGoal) ||
				PropertyName == GET_MEMBER_NAME_CHECKED(FGoapGoalConfigGroupData, bRandByWeight))
		{
			for (auto& GoapGoalConfigGroupData : GoapGoalConfig)
			{
				if(GoapGoalConfigGroupData.bRandBeforSelectGoal && GoapGoalConfigGroupData.bRandByWeight)
				{
					for (auto& GoapGoalConfigData : GoapGoalConfigGroupData.GoapGoalConfigDataList)
					{
						GoapGoalConfigData.bCanConfigWeight = true;
					}
				}
				else if(!GoapGoalConfigGroupData.bRandBeforSelectGoal || !GoapGoalConfigGroupData.bRandByWeight)
				{
					for (auto& GoapGoalConfigData : GoapGoalConfigGroupData.GoapGoalConfigDataList)
					{
						GoapGoalConfigData.bCanConfigWeight = false;
					}
				}
			}
		}
	}
}
#endif

void FGoapGoalSubGroup::ShuffleGroup()
{
	if(GoapGoalSupGroup.Num() <= 1 || !bRandBeforSelectGoal)
		return;


	if(!bRandByWeight)
	{
		for (int Index = GoapGoalSupGroup.Num() - 1; Index > 0; Index--)
		{
			int ToIndex = FMath::RandRange(0, Index);

			GoapGoalSupGroup.Swap(ToIndex, Index);
		}
	}
	else
	{
		int SumWeight = -1;
		for(auto& GoapGoalSupGroupData : GoapGoalSupGroup)
		{
			SumWeight += GoapGoalSupGroupData.Weight;
		}

		for (int Index = GoapGoalSupGroup.Num() - 1; Index > 0; Index--)
		{
			int RandValue = FMath::RandRange(0, SumWeight);

			for (int SwapIndex = 0; SwapIndex <= Index; SwapIndex++)
			{
				RandValue -= GoapGoalSupGroup[SwapIndex].Weight;
				if(RandValue < 0)
				{
					SumWeight -= GoapGoalSupGroup[SwapIndex].Weight;
					GoapGoalSupGroup.Swap(SwapIndex, Index);
					break;
				}
			}
		}
	}

}
