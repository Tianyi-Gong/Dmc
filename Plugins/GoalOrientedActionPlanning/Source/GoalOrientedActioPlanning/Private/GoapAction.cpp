#include "GoapAction.h"
#include "GoapAstarNode.h"

UGoapAction::UGoapAction()
{
    //GoapAstarNode.InitGoapAstarNode(this);
}

UGoapAction::~UGoapAction()
{
#if WITH_EDITOR
    Precondition.UnbindFunction();
    Effect.UnbindFunction();
#endif
}

void UGoapAction::AgentGoapStateChange(const FGoapWorldState& PreGoapWorldState, const FGoapWorldState& CurGoapWorldState)
{
    if (bCanInterruptAction)
    {
        int64 diff = (PreGoapWorldState.Values & ~PreGoapWorldState.NotUsedFlag) ^ (CurGoapWorldState.Values & ~CurGoapWorldState.NotUsedFlag);

        for (auto InterruptActionValue : InterruptActionValueConfig)
        {
            bool CanInterruptAction = false;
            for (auto& [StateKey, StateValue] : InterruptActionValue.StateValueConfig)
            {
                const int DefaultOffset = -1;
                int Offset = CurGoapWorldState.NamesTable.FindRef(StateKey, DefaultOffset);
                if (Offset != DefaultOffset)
                {
                    int64 mask = 1i64 << Offset;

                    if ((diff & mask) == 0)
                    {
                        continue;
                    }

                    if ((bool)(CurGoapWorldState.Values & mask) == StateValue.bStatValue)
                    {
                        CanInterruptAction = true;
                    }
                    else
                    {
                        CanInterruptAction = false;
                        break;
                    }
                }
            }

            if (CanInterruptAction &&
                (GoapActionResult == EGoapActionResult::InProgress ||
                 GoapActionResult == EGoapActionResult::Invalid))
            {
                SetGoapActionResult(EGoapActionResult::Aborted);
            }
        }
    }

    //ReceiveAgentGoapStateChange(PreGoapWorldState,CurGoapWorldState);
}

#if WITH_EDITOR
void UGoapAction::BuildGoapWorldStateData()
{
    Precondition.BuildValueByGoapWorldStateValueConfig(&PreconditionValueConfig);
    Effect.BuildValueByGoapWorldStateValueConfig(&EffectValueConfig);
}
#endif

int UGoapAction::GetCost()
{
    return ConfigCost + StaticCost + DynamicCost;
}

bool UGoapAction::MatchPrecondition(const FGoapWorldState& MatchState)
{
    check(Precondition.GoapWorldStateRuntimeSettingClass == MatchState.GoapWorldStateRuntimeSettingClass)

    int64 care = ~Precondition.NotUsedFlag;
    return (Precondition.Values & care) == (MatchState.Values & care);
}

bool UGoapAction::MatchEffect(const FGoapWorldState& MatchState)
{
    check(Effect.GoapWorldStateRuntimeSettingClass == MatchState.GoapWorldStateRuntimeSettingClass)

    int64 care = (~Effect.NotUsedFlag) & (~MatchState.NotUsedFlag);

    if(care == 0)
        return false;

    return (Effect.Values & care) == (MatchState.Values & care);
}

void UGoapAction::ApplayEffect(FGoapWorldState& WorldState, bool bApplyPrivateState)
{
    check(Effect.GoapWorldStateRuntimeSettingClass == WorldState.GoapWorldStateRuntimeSettingClass)

    int64 mask = Effect.NotUsedFlag | (bApplyPrivateState ? Effect.SharedFlag : ~Effect.SharedFlag);
    int64 NewValues = (WorldState.Values & mask) | (Effect.Values & ~mask);
    //int64 NewValues = (WorldState.Values & Effect.NotUsedFlag) | (Effect.Values & ~Effect.NotUsedFlag);
    
    WorldState.SetNotUsedFlag(WorldState.NotUsedFlag & mask);
    WorldState.SetValues(NewValues);
}

void UGoapAction::ApllayPrecondition(FGoapWorldState& WorldState)
{
    check(Precondition.GoapWorldStateRuntimeSettingClass == WorldState.GoapWorldStateRuntimeSettingClass)

    int64 mask = Precondition.NotUsedFlag;
    int64 NewValues = (WorldState.Values & mask) | (Precondition.Values & ~mask);

    WorldState.SetNotUsedFlag(WorldState.NotUsedFlag & mask);
    WorldState.SetValues(NewValues);

}

//void UGoapAction::ActiveAction()
//{
//   ActionActiveTime = 0;
//    ReceiveActiveAction();
//}

void UGoapAction::UpdateAction(float TickTime)
{
    switch (GoapActionResult)
    {
    case EGoapActionResult::Invalid:
        SetGoapActionResult(EGoapActionResult::InProgress);
        break;
    case EGoapActionResult::InProgress:
        ActionActiveTime += TickTime;
        OnActionTick(TickTime);
        break;
    case EGoapActionResult::Succeeded:
    case EGoapActionResult::Failed:
    case EGoapActionResult::Aborted:
    default:
        break;
    }
}

void UGoapAction::SetGoapActionResult(EGoapActionResult inGoapActionResult)
{
    if(GoapActionResult == inGoapActionResult)
        return;

    GoapActionResult = inGoapActionResult;

    //Todo: Add GoapActionResultChange call back.
    switch (GoapActionResult)
    {
    case EGoapActionResult::Failed:
        OnActionFailed();
        break;
    case EGoapActionResult::Aborted:
        OnActionAborted();
        break;
    case EGoapActionResult::InProgress:
        OnActionActive();
        break;
    case EGoapActionResult::Invalid:
        ActionActiveTime = 0;
    case EGoapActionResult::Succeeded:
    default:
        break;
    }
}

void UGoapAction::EndAction(EGoapActionResult inGoapActionResult)
{
    if(GoapActionResult == inGoapActionResult ||
       GoapActionResult == EGoapActionResult::InProgress ||
       GoapActionResult == EGoapActionResult::Invalid)
       {
        SetGoapActionResult(inGoapActionResult);
       }
}

void UGoapAction::UpdateDynamicCostByGoapState(const FGoapWorldState& GoapWorldState)
{
    ReciveUpdateDynamicCostByGoapState(GoapWorldState);
}

void UGoapAction::OnActionActive()
{
    ActionActiveTime = 0;
    ReciveOnActionActive(AIController, ControlledPawn);
}

void UGoapAction::OnActionTick(float DeltaTime)
{
    ReciveOnActionTick(DeltaTime, AIController, ControlledPawn);
}

void UGoapAction::OnActionAborted()
{
    ReciveOnActionAborted(AIController, ControlledPawn);
}

void UGoapAction::OnActionFailed()
{
    ReciveOnActionFailed(AIController, ControlledPawn);
}

void UGoapAction::GetBlackBoard(UBlackboardComponent*& BlackboardComponent)
{
    BlackboardComponent = AIController->GetBlackboardComponent();
}

#if WITH_EDITOR
void UGoapAction::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
    //Super::PostEditChangeProperty(PropertyChangedEvent);
    //if (PropertyChangedEvent.Property != nullptr)
    //{
    //    const FName PropertyName(PropertyChangedEvent.MemberProperty->GetFName());
    //    if (PropertyName == GET_MEMBER_NAME_CHECKED(UGoapAction, PreconditionValueConfig))
    //    {
    //        Precondition.BuildValueByGoapWorldStateValueConfig(&PreconditionValueConfig);
    //    }
    //    else if(PropertyName == GET_MEMBER_NAME_CHECKED(UGoapAction, EffectValueConfig))
    //    {
    //        Effect.BuildValueByGoapWorldStateValueConfig(&EffectValueConfig);
    //    }
    //}
}

void UGoapAction::PostLoad()
{
    Super::PostLoad();
    //BuildGoapWorldStateData();
}
#endif

void UGoapAction::InitGoapState()
{
    Precondition.InitaData(GoapWorldStateRuntimeSettingClass);
    Precondition.BuildValueByGoapWorldStateValueConfig(&PreconditionValueConfig);

    Effect.InitaData(GoapWorldStateRuntimeSettingClass);
    Effect.BuildValueByGoapWorldStateValueConfig(&EffectValueConfig);
}

void UGoapAction::InitControlerData(AAIController* InAIController)
{
    if(AIController == nullptr)
    {
        AIController = InAIController;
        ControlledPawn = AIController->GetPawn();
    }
}
