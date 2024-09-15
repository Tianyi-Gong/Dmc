#pragma once 
#include "CoreMinimal.h"
#include "GoapAstarNode.h"
#include "GoapAction.h"
#include "GoapActionSet.h"
#include "GoapWorldState.h"
#include "GoapAstarPlanner.generated.h"

UCLASS(config = Game, BlueprintType, Blueprintable)
class GOALORIENTEDACTIONPLANNING_API UGoapAstarPlanner : public UObject
{
	GENERATED_BODY()
public:
	void PlanAction(FGoapWorldState& From, FGoapWorldState& To, UGoapActionSet* ActionSet, TQueue<UGoapAction*>& outPlanAction);

	void GenrateFinalPlan(FGoapAstarNode* GoapAstarEndNode, TQueue<UGoapAction*>& outPlanAction);

	FString GetPlannedActionData(UGoapAction* CurAction);

	UPROPERTY(BlueprintReadOnly)
	TArray<UGoapAction*> PlannedAction;

	UPROPERTY()
	bool bCompareFValueWhenHIsEqual;

	UPROPERTY()
	bool bRandomReturnWhenHIsEqual;

private:
	TArray<FGoapAstarNode*> OpenList;
	TArray<FGoapAstarNode*> CloseList;

	bool FindInOpenList(FGoapAstarNode* GoapAstarNode);
	bool FindInCloseList(FGoapAstarNode* GoapAstarNode);
	void ClearListData();

	virtual bool Compare(const FGoapAstarNode& A, const FGoapAstarNode& B);
};

