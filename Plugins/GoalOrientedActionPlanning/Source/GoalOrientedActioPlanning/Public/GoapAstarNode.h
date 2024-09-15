#pragma once 
#include "CoreMinimal.h"
#include "GoapWorldState.h"
#include <mutex>
#include "GoapAstarNode.generated.h"

class UGoapAction;

USTRUCT()
struct GOALORIENTEDACTIONPLANNING_API FGoapAstarNode
{
	GENERATED_USTRUCT_BODY()

	FGoapAstarNode(FGoapWorldState& CurState, FGoapAstarNode* ParentNode, FGoapWorldState& GoalState, UGoapAction* Owner);
	FGoapAstarNode();
public:
	UPROPERTY()
	int G;

	UPROPERTY()
	int H;

	UPROPERTY()
	int F;

	UPROPERTY()
	FGoapWorldState WorldState;

	FGoapAstarNode* ParentGoapAstarNode;

	void SetCostG(int CostG);

	void BuildGoapAstarNode(FGoapWorldState& CurState, FGoapAstarNode* ParentNode, FGoapWorldState& GoalState, UGoapAction* Owner);
	void InitGoapAstarNode(UGoapAction* inOwner);
	void ClearGoapAstarNodeData();

	int64 GetCareData();

	bool operator==(const FGoapAstarNode& OtherNode) const
	{
		return ((WorldState.Values & ~WorldState.NotUsedFlag) == (OtherNode.WorldState.Values & ~OtherNode.WorldState.NotUsedFlag) &&
		         OwnerPtr == OtherNode.OwnerPtr &&
				 G == G &&
				 H == H
				);
	}

	UGoapAction* GetOwner() { return OwnerPtr;}

private:
	UGoapAction* OwnerPtr;
};

class GoapAstarNodePool
{
public:
	static GoapAstarNodePool* GetInstance();

	FGoapAstarNode* PopGoapAstarNode();
	bool PushGoapAstarNode(FGoapAstarNode* GoapAstarNode);

private:
	GoapAstarNodePool();
	~GoapAstarNodePool();

	GoapAstarNodePool(const GoapAstarNodePool & GoapAstarNodePoolInstance) = delete;
	const GoapAstarNodePool& operator=(const GoapAstarNodePool GoapAstarNodePoolInstance) = delete;

	TArray<FGoapAstarNode*> PopedGoapAstarNodePool;
	TArray<FGoapAstarNode*> CanUsedGoapAstarNodePool;
};

