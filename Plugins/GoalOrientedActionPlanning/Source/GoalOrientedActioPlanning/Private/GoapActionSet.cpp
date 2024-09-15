#include "GoapActionSet.h"
#include "Engine/AssetManager.h"

UGoapActionSet::UGoapActionSet()
{

}

void UGoapActionSet::InitData(AAIController* Controller)
{
	AIController = Controller;

	TArray<FSoftObjectPath> LoadPath;

	for(auto GoapAction : GoapActionConfig)
	{
		//GoapActionPool.Add(NewObject<UGoapAction>(this ,GoapAction));

		LoadPath.Add(FSoftObjectPath(GoapAction->GetClassPathName()));
	}

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	Streamable.RequestAsyncLoad(LoadPath,FStreamableDelegate::CreateLambda([this]()
	{
		GoapActionPool.Empty();

		for (auto GoapActionClass : GoapActionConfig)
		{
			UGoapAction* GoapAction = NewObject<UGoapAction>(GetOuter(), GoapActionClass);
			if(GoapAction)
			{
				check(GoapWorldStateRuntimeSettingClass == GoapAction->GoapWorldStateRuntimeSettingClass)
				if(GoapWorldStateRuntimeSettingClass == GoapAction->GoapWorldStateRuntimeSettingClass)
				{
					GoapAction->InitGoapState();
					GoapAction->InitControlerData(AIController);
					GoapActionPool.Add(GoapAction);
				}

			}
		}

		bIsDtatLoaded = true;
	}));
}

void UGoapActionSet::UpdateCanUsedGoapActionPool(FGameplayTag CanUsedGoapActionType,const FGoapWorldState& GoapWorldState)
{
	CanUsedGoapActionPool.Empty();

	for (auto GoapAction : GoapActionPool)
	{
		if(!GoapAction->GoapActionType.IsValid() ||
		   !CanUsedGoapActionType.IsValid() ||
		   CanUsedGoapActionType.MatchesTag(GoapAction->GoapActionType))
		{
			//GoapAction->GetGoapAstarNode()->ClearGoapAstarNodeData();
			GoapAction->UpdateDynamicCostByGoapState(GoapWorldState);
			CanUsedGoapActionPool.Add(GoapAction);

			GoapAction->SetGoapActionResult(EGoapActionResult::Invalid);
		}
	}
}

bool UGoapActionSet::GetPossibleTrans(FGoapAstarNode* CurNode, FGoapWorldState& From, FGoapWorldState& To, TArray<FGoapAstarNode *>& outGoapActionArray)
{
	const FGoapWorldState& CurState = CurNode->WorldState;

	for (auto GoapAction : CanUsedGoapActionPool)
	{
		if(GoapAction->MatchEffect(CurState))
		{
			FGoapWorldState NextState = CurState;
			GoapAction->ApllayPrecondition(NextState);

			GoapAstarNodePool* GoapAstarNodePoolPtr = GoapAstarNodePool::GetInstance();
			FGoapAstarNode* GoapAstarNodePtr = GoapAstarNodePoolPtr->PopGoapAstarNode();
			GoapAstarNodePtr->BuildGoapAstarNode(NextState, CurNode, From, GoapAction);

			outGoapActionArray.Add(GoapAstarNodePtr);
		}
	}

	return !outGoapActionArray.IsEmpty();
}

int UGoapActionSet::CacheCanUsedGoapActionPoolToString(FString& outString)
{
	if (CanUsedGoapActionPool.Num() > 0)
	{
		for (auto CanUsedGoapAction : CanUsedGoapActionPool)
		{
			outString += "  " + CanUsedGoapAction->GetClass()->GetName() + "\n";
		}
	}
	else
	{
		outString += "  Empty";
	}

	return CanUsedGoapActionPool.Num();
}
