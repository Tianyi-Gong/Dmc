#include "GoapAstarPlanner.h"
#include "Debug/StatDefine.h"

void UGoapAstarPlanner::PlanAction(FGoapWorldState& From, FGoapWorldState& To, UGoapActionSet* ActionSet, TQueue<UGoapAction*>& outPlanAction)
{
	SCOPE_CYCLE_COUNTER(STAT_GoapPlanAction);

	outPlanAction.Empty();
	ClearListData();

	GoapAstarNodePool* GoapAstarNodePoolPtr = GoapAstarNodePool::GetInstance();

	FGoapAstarNode* GoapAstarFirstNode = GoapAstarNodePoolPtr->PopGoapAstarNode();
	GoapAstarFirstNode->BuildGoapAstarNode(To, nullptr, From, nullptr);

	OpenList.HeapPush( GoapAstarFirstNode,
						[this](const FGoapAstarNode& A, const FGoapAstarNode& B)
						{
							return Compare(A,B);
						});

	while (!OpenList.IsEmpty())
	{
		FGoapAstarNode* CurGoapAstartNode = nullptr;
		OpenList.HeapPop( CurGoapAstartNode,
		                  [this](const FGoapAstarNode& A, const FGoapAstarNode& B)
						  {
							return Compare(A,B);
						  });

		CloseList.Add(CurGoapAstartNode);

		int64 CurrentCare = ~CurGoapAstartNode->WorldState.NotUsedFlag;

		if( (CurGoapAstartNode->WorldState.Values & CurrentCare) == (From.Values & CurrentCare) )
		{
			GenrateFinalPlan(CurGoapAstartNode ,outPlanAction);
			ClearListData();
			return;
		}

		TArray<FGoapAstarNode *> PossibleGoapAstarNode;

		ActionSet->GetPossibleTrans(CurGoapAstartNode, From, To, PossibleGoapAstarNode);

		for (auto GoapAstarNode : PossibleGoapAstarNode)
		{			
			if(FindInCloseList(GoapAstarNode))
			{
				GoapAstarNodePoolPtr->PushGoapAstarNode(GoapAstarNode);
				continue;
			}

			int CostG = CurGoapAstartNode->G + GoapAstarNode->GetOwner()->GetCost();
			GoapAstarNode->SetCostG(CostG);

			if(!FindInOpenList(GoapAstarNode))
			{
				OpenList.HeapPush( 
					GoapAstarNode,
					[this](const FGoapAstarNode& A, const FGoapAstarNode& B)
					{
						return Compare(A, B);
					});
			}
			else
			{
				GoapAstarNodePoolPtr->PushGoapAstarNode(GoapAstarNode);
			}
		}
	}
}

void UGoapAstarPlanner::GenrateFinalPlan(FGoapAstarNode* GoapAstarEndNode, TQueue<UGoapAction*>& outPlanAction)
{
	outPlanAction.Empty();
	PlannedAction.Empty();

	if (GoapAstarEndNode->ParentGoapAstarNode == nullptr)
	{
		return;
	}

	FGoapAstarNode* GoapAstarParentNode = GoapAstarEndNode;

	while (GoapAstarParentNode->ParentGoapAstarNode != nullptr)
	{
		PlannedAction.Add(GoapAstarParentNode->GetOwner()); 
		outPlanAction.Enqueue(GoapAstarParentNode->GetOwner());
		GoapAstarParentNode = GoapAstarParentNode->ParentGoapAstarNode;
	}
}

FString UGoapAstarPlanner::GetPlannedActionData(UGoapAction* CurAction)
{
	FString PlannedActionDataString;

	for (auto Action : PlannedAction)
	{
		bool bIsCurAction = (Action == CurAction);

		PlannedActionDataString += bIsCurAction ? "{green}  " : "{white}  ";
		PlannedActionDataString += Action->GetClass()->GetName();
		PlannedActionDataString += FString::Printf(TEXT("  ActiveTime: %.2f\n"), Action->ActionActiveTime);

	}
	return PlannedActionDataString;
}

bool UGoapAstarPlanner::FindInOpenList(FGoapAstarNode* GoapAstarNode)
{
	for (auto GoapAstarNodePtr : OpenList)
	{
		if(*(GoapAstarNodePtr) == *(GoapAstarNode))
		{
			return true;
		}
	}

	return false;
}

bool UGoapAstarPlanner::FindInCloseList(FGoapAstarNode* GoapAstarNode)
{
	for (auto GoapAstarNodePtr : CloseList)
	{
		if (*(GoapAstarNodePtr) == *(GoapAstarNode))
		{
			return true;
		}
	}

	return false;
}

void UGoapAstarPlanner::ClearListData()
{
	GoapAstarNodePool* GoapAstarNodePoolPtr = GoapAstarNodePool::GetInstance();
	if(!OpenList.IsEmpty())
	{
		for (auto GoapAstarNodePtr : OpenList)
		{
			GoapAstarNodePoolPtr->PushGoapAstarNode(GoapAstarNodePtr);
		}

		OpenList.Empty();
	}

	if (!CloseList.IsEmpty())
	{
		for (auto GoapAstarNodePtr : CloseList)
		{
			GoapAstarNodePoolPtr->PushGoapAstarNode(GoapAstarNodePtr);
		}

		CloseList.Empty();
	}
}

bool UGoapAstarPlanner::Compare(const FGoapAstarNode& A, const FGoapAstarNode& B)
{
	if (bCompareFValueWhenHIsEqual && A.F == B.F)
	{
		if(bRandomReturnWhenHIsEqual)
		{
			return FMath::RandBool();
		}
		else
		{
			return A.H < B.H;
		}
		
	}

	return A.F < B.F;
}
