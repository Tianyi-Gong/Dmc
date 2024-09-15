#include "GoapGoal.h"
#include "GoapAgentComponent.h"

UGoapGoal::~UGoapGoal()
{
#ifdef WITH_EDITOR
	Goal.UnbindFunction();
	Precondition.UnbindFunction();
#endif // WITH_EDITOR
}

void UGoapGoal::SetGoapGoalResult(const EGoapGoalResult inGoapGoalResult)
{
	if(GoapGoalResult == inGoapGoalResult)
		return;

	GoapGoalResult = inGoapGoalResult;

	if( UGoapAgentComponent* GoapAgentComponent = Cast<UGoapAgentComponent>( GetOuter() ) )
	{
		switch (GoapGoalResult)
		{
			case EGoapGoalResult::Succeeded:
			case EGoapGoalResult::Failed:
			case EGoapGoalResult::Aborted:
			//Todo: Unregister GoapPrivateState value call back.
				break;
			case EGoapGoalResult::InProgress:
			//Todo: Register GoapPrivateState value call back. 
				break;
			case EGoapGoalResult::Invalid:
			default:
				break;
		}
	}
}

void UGoapGoal::OnGoalActive(UGoapAgentComponent* GoapAgent)
{
	ReciveOnGoalActive(GoapAgent,AIController,ControlledPawn);
}

void UGoapGoal::AgentGoapStateChange(const FGoapWorldState& PreGoapWorldState, const FGoapWorldState& CurGoapWorldState)
{
	if(bCanInterruptGoal)
	{
		int64 diff = (PreGoapWorldState.Values & ~PreGoapWorldState.NotUsedFlag) ^ (CurGoapWorldState.Values & ~CurGoapWorldState.NotUsedFlag);

		for (auto InterruptGoalValue : InterruptGoalValueConfig)
		{
			bool CanInterruptGoal = false;
			for (auto& [StateKey, StateValue] : InterruptGoalValue.StateValueConfig)
			{
				const int DefaultOffset = -1;				
				int Offset = CurGoapWorldState.NamesTable.FindRef(StateKey, DefaultOffset);
				if(Offset != DefaultOffset)
				{
					int64 mask = 1i64 << Offset;

					if((diff & mask) == 0)
					{
						continue;
					}

					if( (bool)(CurGoapWorldState.Values & mask) == StateValue.bStatValue)
					{
						CanInterruptGoal = true;
					}
					else
					{
						CanInterruptGoal = false;
						break;
					}
				}
			}

			if(CanInterruptGoal &&
			  (GoapGoalResult == EGoapGoalResult::InProgress ||
			   GoapGoalResult == EGoapGoalResult::Invalid))
			{
				SetGoapGoalResult(EGoapGoalResult::Aborted);
			}
		}
		
	}

	//ReceiveAgentGoapStateChange(PreGoapWorldState, CurGoapWorldState);
}

#if WITH_EDITOR
void UGoapGoal::BuildGoapWorldStateData()
{
	Goal.BuildValueByGoapWorldStateValueConfig(&GoalValueConfig);
	Precondition.BuildValueByGoapWorldStateValueConfig(&PreconditionValueConfig);
}


void UGoapGoal::PostLoad()
{
	Super::PostLoad();

	//BuildGoapWorldStateData();
}

void UGoapGoal::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	//if (PropertyChangedEvent.Property != nullptr)
	//{
	//	const FName PropertyName(PropertyChangedEvent.MemberProperty->GetFName());
	//	if (PropertyName == GET_MEMBER_NAME_CHECKED(UGoapGoal, GoalValueConfig))
	//	{
	//		BuildGoapWorldStateData();
	//	}
	//	else if(PropertyName == GET_MEMBER_NAME_CHECKED(UGoapGoal, PreconditionValueConfig))
	//	{
	//		BuildGoapWorldStateData();
	//	}
	//}
}
#endif

void UGoapGoal::InitGoapState()
{
	Goal.InitaData(GoapWorldStateRuntimeSettingClass);
	Goal.BuildValueByGoapWorldStateValueConfig(&GoalValueConfig);

	Precondition.InitaData(GoapWorldStateRuntimeSettingClass);
	Precondition.BuildValueByGoapWorldStateValueConfig(&PreconditionValueConfig);
}

void UGoapGoal::InitControlerData(AAIController* InAIController)
{
	if (AIController != InAIController)
	{
		AIController = InAIController;
		ControlledPawn = AIController->GetPawn();
	}
}
