#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GoapWorldState.h"
#include "GoapAstarNode.h"
#include "Runtime\AIModule\Classes\AIController.h"
#include "Runtime\AIModule\Classes\BehaviorTree\BlackboardComponent.h"
#include "GoapAction.generated.h"

UENUM(BlueprintType)
enum class EGoapActionResult : uint8
{
	Invalid,
	Succeeded,
	Failed,
	Aborted,
	InProgress
};

UCLASS(config = Game, BlueprintType, Blueprintable)
class GOALORIENTEDACTIONPLANNING_API UGoapAction : public UObject
{
	GENERATED_BODY()
public:
	UGoapAction();
	~UGoapAction();

	UPROPERTY(EditAnywhere)
	int ConfigCost;

	UPROPERTY()
	int StaticCost;

	UPROPERTY()
	int DynamicCost;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGoapWorldStateRuntimeSettings> GoapWorldStateRuntimeSettingClass;

	UPROPERTY(EditAnywhere)
	FGameplayTag GoapActionType;

	UPROPERTY(EditAnywhere)
	FGoapWorldStateValueConfig PreconditionValueConfig;

	UPROPERTY()
	FGoapWorldState Precondition;

	UPROPERTY(EditAnywhere)
	FGoapWorldStateValueConfig EffectValueConfig;

	UPROPERTY()
	FGoapWorldState Effect;

	UPROPERTY(EditAnywhere)
	bool bCanInterruptGoal;

	UPROPERTY(EditAnywhere, meta=(EditCondition="bCanInterruptGoal", EditConditionHides))
	TArray<FGoapWorldStateValueConfig> InterruptGoalValueConfig;

	virtual void AgentGoapStateChange(const FGoapWorldState& PreGoapWorldState, const FGoapWorldState& CurGoapWorldState);
	//UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "AgentGoapStateChange"))
	//void ReceiveAgentGoapStateChange(const FGoapWorldState& PreGoapWorldState, const FGoapWorldState& CurGoapWorldState);

#if WITH_EDITOR
	void BuildGoapWorldStateData();
#endif

	UFUNCTION()
	int GetCost();

	bool MatchPrecondition(const FGoapWorldState& MatchState);
	bool MatchEffect(const FGoapWorldState& MatchState);

	void ApplayEffect(FGoapWorldState& WorldState, bool ApplyPrivateState = true);

	void ApllayPrecondition(FGoapWorldState& WorldState);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "UpdateGoapAction"))
	void ReceiveUpdateAction(float TickTime);

	virtual void UpdateAction(float TickTime);
	EGoapActionResult GetGoapActionResult() { return GoapActionResult; }
	void SetGoapActionResult(EGoapActionResult inGoapActionResult);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "EndGoapAction"))
	void EndAction(EGoapActionResult GoapActionResult);

	virtual void UpdateDynamicCostByGoapState(const FGoapWorldState& GoapWorldState);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Update Dynamic Cost By GoapState"))
	void ReciveUpdateDynamicCostByGoapState(const FGoapWorldState& GoapWorldState);

	//virtual void ActiveAction();
	//UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ActiveGoapAction"))
	//void ReceiveActiveAction(AAIController* InAIController, APawn* InControlledPawn);

	virtual void OnActionActive();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Action Active"))
	void ReciveOnActionActive(AAIController* InAIController, APawn* InControlledPawn);

	virtual void OnActionTick(float DeltaTime);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Action Tick"))
	void ReciveOnActionTick(float DeltaTime, AAIController* InAIController, APawn* InControlledPawn);

	virtual void OnActionAborted();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Action Aborted"))
	void ReciveOnActionAborted(AAIController* InAIController, APawn* InControlledPawn);

	virtual void OnActionFailed();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Action Failed"))
	void ReciveOnActionFailed(AAIController* InAIController, APawn* InControlledPawn);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetBlackBoard(UBlackboardComponent*& Blackboard);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostLoad() override;
#endif	
	float ActionActiveTime;

	void InitGoapState();
	virtual void InitControlerData(AAIController* InAIController);
protected:
	EGoapActionResult GoapActionResult = EGoapActionResult::Invalid;
	AAIController* AIController;
	APawn* ControlledPawn;
};

