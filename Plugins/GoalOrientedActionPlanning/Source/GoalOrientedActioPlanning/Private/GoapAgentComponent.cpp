#include "GoapAgentComponent.h"
#include "GoapSubsystem.h"
#include "Engine/AssetManager.h"
#include "Runtime\AIModule\Classes\BehaviorTree\BlackboardComponent.h"
#include "Debug/StatDefine.h"

UGoapAgentComponent::UGoapAgentComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	GoapAstarPlanner = CreateDefaultSubobject<UGoapAstarPlanner>(TEXT("GoapAstarPlanner"));
}

void UGoapAgentComponent::GoapPrivateStateMerageGoapWorldState(const FGoapWorldState& GoapPrivateState, const FGoapWorldState& GoapWorldState, FGoapWorldState& outGoapMerageState)
{
	outGoapMerageState.SetValues(( GoapPrivateState.Values & ~GoapPrivateState.SharedFlag ) | ( GoapWorldState.Values & GoapPrivateState.SharedFlag ));
	outGoapMerageState.SetNotUsedFlag((GoapPrivateState.NotUsedFlag & ~GoapPrivateState.SharedFlag) | (GoapWorldState.NotUsedFlag & GoapPrivateState.SharedFlag));
}

void UGoapAgentComponent::SetGoapPrivateState(FGameplayTag GamePlayTag, bool Value)
{
	GoapPrivateState.SetStateValue(GamePlayTag, Value);
}

bool UGoapAgentComponent::GetGoapPrivateState(FGameplayTag GamePlayTag, bool& Value, bool IgnoreUsedFlag)
{
	return GoapPrivateState.GetStateValue(GamePlayTag, Value, IgnoreUsedFlag);
}

void UGoapAgentComponent::UpdateGoapPrivateState(float DeltaTime)
{
	ReceiveUpdateGoapPrivateState(DeltaTime);
}

void UGoapAgentComponent::OnPreGoapGoalSelect()
{
	ReceiveOnPreGoapGoalSelect(this);

	OnPreGoapGoalSelectDelegate.Broadcast(this);
}

void UGoapAgentComponent::OnGoapGoalSelect()
{
	ReceiveOnGoapGoalSelect(this);

	OnGoapGoalSelectDelegate.Broadcast(this);
	if(CurGoapGoal)
	{
		CurGoapGoal->OnGoalActive(this);
	}
}

void UGoapAgentComponent::BeginPlay()
{
	Super::BeginPlay();

	if(AAIController* Controller = Cast<AAIController>(GetOwner()))
	{
		InitAIController(Controller);

		StartAsyncLoadData();
		InitGoapState();
	}
	else
	{
		check(Controller != nullptr)
	}
}

void UGoapAgentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_GoapAgentTick);

	if (!bIsInitialize)
		return;

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(GoapActionSet == nullptr || !GoapActionSet->IsDataFullLoaded())
	{
		UE_LOG(LogTemp, Log, TEXT("GOAP: GoapActionSet is not Init."));
		return;
	}

	if(GoapGoalSet == nullptr || !GoapGoalSet->IsDataFullLoaded())
	{
		UE_LOG(LogTemp, Log, TEXT("GOAP: GoapGoalSet is not Init."));
		return;
	}

	UpdateGoapWorldState(DeltaTime);
	UpdateGoapGoalState(DeltaTime);
	UpdateGoapActionState(DeltaTime);
}

void UGoapAgentComponent::UpdateGoapWorldState(float DeltaTime)
{
	PreGoapState = CurGoapState;
	UpdateGoapPrivateState(DeltaTime);

	const FGoapWorldState& CurGoapWorldState = UGoapSubsystem::GetGoapSubsystem(GWorld)->GetGoapWorldState(GoapWorldStateRuntimeSettingClass);
	GoapPrivateStateMerageGoapWorldState(GoapPrivateState, CurGoapWorldState, CurGoapState);
}

void UGoapAgentComponent::UpdateGoapGoalState(float DeltaTime)
{
	if (CurGoapGoal != nullptr && CurGoapGoal->GetGoapGoalResult() == EGoapGoalResult::InProgress)
	{
		if (CurGoapState != PreGoapState)
		{
			CurGoapGoal->AgentGoapStateChange(PreGoapState, CurGoapState);
		}
	}

	if ( CurGoapGoal == nullptr ||
		 CurGoapGoal->GetGoapGoalResult() == EGoapGoalResult::Aborted ||
		 CurGoapGoal->GetGoapGoalResult() == EGoapGoalResult::Failed ||
		 CurGoapGoal->GetGoapGoalResult() == EGoapGoalResult::Succeeded )
	{
		if( CurGoapGoal != nullptr )
		{
			CurGoapGoal->SetGoapGoalResult(EGoapGoalResult::Invalid);
		}
		OnPreGoapGoalSelect();
		CurGoapGoal = GoapGoalSet->SelectGoalByWorldState(CurGoapState);
		if(CurGoapGoal != nullptr)
		{
			OnGoapGoalSelect();
		}

		if( CurActiveAction != nullptr &&
			CurActiveAction->GetGoapActionResult() == EGoapActionResult::InProgress )
		{
			CurActiveAction->EndAction(EGoapActionResult::Aborted);	
		}
		CurActiveAction = nullptr;

		if(!PlannAction.IsEmpty())
		{
			PlannAction.Empty();
		}
	}

	if (CurGoapGoal != nullptr)
	{
		if (PlannAction.IsEmpty() && CurActiveAction == nullptr)
		{
			GoapActionSet->UpdateCanUsedGoapActionPool(CurGoapGoal->GoapGoalCanUsedActionType, CurGoapState);

			GoapAstarPlanner->PlanAction(CurGoapState, CurGoapGoal->Goal, GoapActionSet, PlannAction);

			if(PlannAction.IsEmpty())
			{
				CurGoapGoal->SetGoapGoalResult(EGoapGoalResult::Failed);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("GOAP: CurGoapGoal is nullptr."));
	}
}

void UGoapAgentComponent::UpdateGoapActionState(float DeltaTime)
{
	if(CurActiveAction == nullptr && !PlannAction.IsEmpty())
	{
		PlannAction.Dequeue(CurActiveAction);

		CurActiveAction->SetGoapActionResult(EGoapActionResult::Invalid);
	}

	if (CurActiveAction != nullptr && CurActiveAction->GetGoapActionResult() == EGoapActionResult::InProgress)
	{
		if (CurGoapState != PreGoapState)
		{
			CurActiveAction->AgentGoapStateChange(PreGoapState, CurGoapState);
		}
	}

	if( CurActiveAction == nullptr )
	{
		UE_LOG(LogTemp, Log, TEXT("GOAP: CurActiveAction is nullptr."));
		return;
	}

	if(CurActiveAction->GetGoapActionResult() == EGoapActionResult::Succeeded)
	{
		CurActiveAction->ApplayEffect(GoapPrivateState);
		//CurActiveAction->ApplayEffect(UGoapSubsystem::GetGoapSubsystem(GWorld)->GetGoapWorldState(),false);
		UGoapSubsystem::GetGoapSubsystem(GWorld)->WorldShardStateAppllyEffect(CurActiveAction->Effect);

		CurActiveAction = nullptr;

		if(PlannAction.IsEmpty())
		{
			CurGoapGoal->SetGoapGoalResult(EGoapGoalResult::Succeeded);
		}
	}
	else if( CurActiveAction->GetGoapActionResult() == EGoapActionResult::Failed ||
			 CurActiveAction->GetGoapActionResult() == EGoapActionResult::Aborted )
	{
		PlannAction.Empty();
		CurGoapGoal->SetGoapGoalResult(CurActiveAction->GetGoapActionResult() == EGoapActionResult::Failed ? EGoapGoalResult::Failed : EGoapGoalResult::Aborted);
		CurActiveAction = nullptr;
	}
	else
	{
		CurActiveAction->UpdateAction(DeltaTime);
	}
}

void UGoapAgentComponent::StartAsyncLoadData()
{
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	if(GoapActionSetConfig.Get() != nullptr)
	{
		TArray<FSoftObjectPath> LoadPathList;

		if(GoapActionSetConfig.Get() != nullptr)
			LoadPathList.Add(FSoftObjectPath(GoapActionSetConfig->GetClassPathName()));

		if(GoapGoalSetConfig.Get() != nullptr)
			LoadPathList.Add(FSoftObjectPath(GoapGoalSetConfig->GetClassPathName()));

		Streamable.RequestAsyncLoad(
			LoadPathList,
			FStreamableDelegate::CreateLambda([this]()
			{
				if (GoapActionSetConfig.Get() != nullptr)
				{
					GoapActionSet = NewObject<UGoapActionSet>(this, GoapActionSetConfig);
					if (GoapActionSet != nullptr)
					{
						GoapActionSet->InitData(AIController);
					}
				}

				if(GoapGoalSetConfig.Get() != nullptr)
				{
					GoapGoalSet = NewObject<UGoapGoalSet>(this, GoapGoalSetConfig);
					if(GoapGoalSet != nullptr)
					{
						GoapGoalSet->InitData(AIController);
					}
				}
			}));
	}

	GoapAstarPlanner->bCompareFValueWhenHIsEqual = bCompareFValueWhenHIsEqual;
	GoapAstarPlanner->bRandomReturnWhenHIsEqual = bRandomReturnWhenHIsEqual;
}

FString UGoapAgentComponent::GoapPrivateStateToString()
{
	return GoapPrivateState.ToString();
}

void UGoapAgentComponent::InitGoapState()
{
	GoapPrivateState.InitaData(GoapWorldStateRuntimeSettingClass);
	PreGoapState.InitaData(GoapWorldStateRuntimeSettingClass);
	CurGoapState.InitaData(GoapWorldStateRuntimeSettingClass);
}

void UGoapAgentComponent::InitAIController(AAIController* Controller)
{
	AIController = Controller;
	bIsInitialize = true;

	UBlackboardComponent* BlackboardComponent = nullptr;
	if(BlackboardAsset != nullptr && AIController != nullptr)
	{
		bool Result = AIController->UseBlackboard(BlackboardAsset, BlackboardComponent);

		check(Result);
	}
}
