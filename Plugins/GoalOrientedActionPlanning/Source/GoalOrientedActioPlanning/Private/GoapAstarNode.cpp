#include "GoapAstarNode.h"
#include "GoapAction.h"

FGoapAstarNode::FGoapAstarNode(FGoapWorldState& CurState, FGoapAstarNode* ParentNode, FGoapWorldState& GoalState, UGoapAction* Owner)
{
	G = 0;
	H = 0;
	F = 0;
	BuildGoapAstarNode(CurState,ParentNode,GoalState, Owner);
}

FGoapAstarNode::FGoapAstarNode()
{
	G = 0;
	H = 0;
	F = 0;
	OwnerPtr = nullptr;
}

void FGoapAstarNode::SetCostG(int CostG)
{
	G = CostG;
	F = G + H;
}

void FGoapAstarNode::BuildGoapAstarNode(FGoapWorldState& CurState, FGoapAstarNode* ParentNode, FGoapWorldState& GoalState, UGoapAction* Owner)
{
	InitGoapAstarNode(Owner);
	WorldState = CurState;
	ParentGoapAstarNode = ParentNode;

	H = GoalState.CalcCorrelation(WorldState);
	SetCostG(G);
}

void FGoapAstarNode::InitGoapAstarNode(UGoapAction* inOwner)
{
	OwnerPtr = inOwner;
}

void FGoapAstarNode::ClearGoapAstarNodeData()
{
	H = 0;
	G = 0;
	F = 0;
	WorldState.Clear();
	ParentGoapAstarNode = nullptr;
}

int64 FGoapAstarNode::GetCareData()
{
	return WorldState.Values & ~WorldState.NotUsedFlag;
}

static GoapAstarNodePool* GoapAstarNodePoolInstance = nullptr;
static std::mutex GoapAstarNodeMutex;

GoapAstarNodePool* GoapAstarNodePool::GetInstance()
{
	if(GoapAstarNodePoolInstance == nullptr)
	{
		std::unique_lock<std::mutex> lock(GoapAstarNodeMutex);
		if(GoapAstarNodePoolInstance == nullptr)
		{
			GoapAstarNodePoolInstance = new GoapAstarNodePool();
		}
	}

	return GoapAstarNodePoolInstance;
}

FGoapAstarNode* GoapAstarNodePool::PopGoapAstarNode()
{
	FGoapAstarNode* PopedGoapAstarNode = nullptr;
	if(CanUsedGoapAstarNodePool.IsEmpty())
	{
		PopedGoapAstarNode = new FGoapAstarNode();
		
	}
	else
	{
		PopedGoapAstarNode = CanUsedGoapAstarNodePool.Pop();
	}
	PopedGoapAstarNodePool.Push(PopedGoapAstarNode);

	return PopedGoapAstarNode;
}

bool GoapAstarNodePool::PushGoapAstarNode(FGoapAstarNode* GoapAstarNode)
{
	if(PopedGoapAstarNodePool.Contains(GoapAstarNode))
	{
		PopedGoapAstarNodePool.Remove(GoapAstarNode);
		CanUsedGoapAstarNodePool.AddUnique(GoapAstarNode);

		return true;
	}

	return false;
}

GoapAstarNodePool::GoapAstarNodePool()
{

}

GoapAstarNodePool::~GoapAstarNodePool()
{

}
