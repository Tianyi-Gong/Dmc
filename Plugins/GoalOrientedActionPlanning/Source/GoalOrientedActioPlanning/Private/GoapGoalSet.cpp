#include "GoapGoalSet.h"
#include "Engine/AssetManager.h"

void UGoapGoalSet::InitData(AAIController* Controller)
{
	AIController = Controller;

	TArray<FSoftObjectPath> LoadPath;

	for (auto GoapGoalConfigData : GoapGoalConfig)
	{
		for (auto LoadGoapGoalClass : GoapGoalConfigData.GoapGoalClass)
		{
			LoadPath.Add(FSoftObjectPath(LoadGoapGoalClass->GetClassPathName()));
		}
	}

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	Streamable.RequestAsyncLoad(LoadPath, FStreamableDelegate::CreateLambda( 
		[this]()
		{
			GoapGoalPool.Empty();

			for (auto GoapGoalConfigData : GoapGoalConfig)
			{
				FGoapGoalSubGroup GoapGoalSubGroup;
				for (auto LoadGoapGoalClass : GoapGoalConfigData.GoapGoalClass)
				{
					UGoapGoal* GoapGoal = NewObject<UGoapGoal>(GetOuter(), LoadGoapGoalClass);

					if (GoapGoal)
					{
						check(GoapGoal->GoapWorldStateRuntimeSettingClass == GoapWorldStateRuntimeSettingClass)
							if (GoapGoal->GoapWorldStateRuntimeSettingClass == GoapWorldStateRuntimeSettingClass)
							{
								GoapGoal->InitGoapState();
								GoapGoal->InitControlerData(AIController);
								GoapGoalSubGroup.GoapGoalSupGroup.Add(GoapGoal);
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
		for(auto GoapGoal : GoapSubGroup.GoapGoalSupGroup)
		{
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
		if (PropertyName == GET_MEMBER_NAME_CHECKED(FGoapGoalConfigData, Index))
		{
			for (auto& GoapGoalConfigData : GoapGoalConfig)
			{
				if(GoapGoalConfigData.Index * 2 != GoapGoalConfigData.Priority)
				{
					GoapGoalConfigData.Priority = (GoapGoalConfigData.Index - 1) * 2 + 1;
				}
			}

			GoapGoalConfig.Sort(
				[](const FGoapGoalConfigData& A, const FGoapGoalConfigData& B)
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
				[](const FGoapGoalConfigData& A, const FGoapGoalConfigData& B)
				{
					return A.Priority < B.Priority;
				});
		}
	}
}
#endif

void FGoapGoalSubGroup::ShuffleGroup()
{
	if(GoapGoalSupGroup.Num() <= 1)
		return;

	for (int index = GoapGoalSupGroup.Num() - 1; index > 0; index--)
	{
		int ToIndex = FMath::RandRange(0, index);

		GoapGoalSupGroup.Swap(ToIndex, index);
	}
}
