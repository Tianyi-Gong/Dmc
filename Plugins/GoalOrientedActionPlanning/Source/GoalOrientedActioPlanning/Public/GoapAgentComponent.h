#pragma once 
#include "CoreMinimal.h"
#include "GoapWorldState.h"
#include "GoapActionSet.h"
#include "GoapAstarPlanner.h"
#include "GoapGoalSet.h"
#include "Runtime\AIModule\Classes\AIController.h"
#include "Runtime\AIModule\Classes\BehaviorTree\BlackboardData.h"
#include "GoapAgentComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoapGoalSelectDelegate, UGoapAgentComponent* , GoapAgent);

UCLASS(config = Game, BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class GOALORIENTEDACTIONPLANNING_API UGoapAgentComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UGoapAgentComponent();

	UFUNCTION(BlueprintCallable)
	static void GoapPrivateStateMerageGoapWorldState(const FGoapWorldState& GoapPrivateState, const FGoapWorldState& GoapWorldState, FGoapWorldState& outGoapMerageState);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGoapWorldStateRuntimeSettings> GoapWorldStateRuntimeSettingClass;

	UPROPERTY(VisibleAnywhere)
	FGoapWorldState GoapPrivateState;

	UPROPERTY(VisibleAnywhere)
	FGoapWorldState CurGoapState;

	UPROPERTY(VisibleAnywhere)
	FGoapWorldState PreGoapState;

	UPROPERTY(Transient,VisibleAnywhere)
	UGoapActionSet* GoapActionSet;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGoapActionSet> GoapActionSetConfig;

	UPROPERTY(VisibleAnywhere)
	UGoapAstarPlanner* GoapAstarPlanner;

	UPROPERTY(Transient,VisibleAnywhere)
	UGoapGoalSet* GoapGoalSet;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGoapGoalSet> GoapGoalSetConfig;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBlackboardData> BlackboardAsset;

	UPROPERTY(Transient)
	UGoapGoal* CurGoapGoal;

	TQueue<UGoapAction*> PlannAction;

	UPROPERTY(Transient)
	UGoapAction* CurActiveAction;

	UPROPERTY(EditAnywhere)
	bool bCompareFValueWhenHIsEqual;

	UPROPERTY(EditAnywhere,meta = (EditCondition = "bCompareFValueWhenHIsEqual", EditConditionHides))
	bool bRandomReturnWhenHIsEqual;

	UFUNCTION(BlueprintCallable)
	void SetGoapPrivateState(FGameplayTag GamePlayTag, bool Value);

	virtual void UpdateGoapPrivateState(float DeltaTime);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "UpdateGoapPrivateState"))
	void ReceiveUpdateGoapPrivateState(float DeltaTime);

	virtual void OnGoapGoalSelect();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnGoapGoalSelect"))
	void ReceiveOnGoapGoalSelect(UGoapAgentComponent* GoapAgent);

	UPROPERTY(BlueprintAssignable)
	FOnGoapGoalSelectDelegate OnGoapGoalSelectDelegate;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void UpdateGoapWorldState(float DeltaTime);
	virtual void UpdateGoapGoalState(float DeltaTime);
	virtual void UpdateGoapActionState(float DeltaTime);

	void StartAsyncLoadData();

	UFUNCTION(BlueprintCallable)
	FString GoapPrivateStateToString();

	void InitGoapState();
	void InitAIController(AAIController* Controller);

	UPROPERTY(BlueprintReadWrite)
	AAIController* AIController;
private:
	
	bool bIsInitialize;
};