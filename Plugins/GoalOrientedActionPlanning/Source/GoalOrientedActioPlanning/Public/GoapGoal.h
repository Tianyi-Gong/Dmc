#pragma once 
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GoapWorldState.h"
#include "GoapGoal.generated.h"

UENUM(BlueprintType)
enum class EGoapGoalResult : uint8
{
	Invalid,
	Succeeded,
	Failed,
	Aborted,
	InProgress
};

UCLASS(config = Game, BlueprintType, Blueprintable)
class GOALORIENTEDACTIONPLANNING_API UGoapGoal : public UObject
{
	GENERATED_BODY()
public:
	~UGoapGoal();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGoapWorldStateRuntimeSettings> GoapWorldStateRuntimeSettingClass;

	UPROPERTY(EditAnywhere)
	FGoapWorldStateValueConfig PreconditionValueConfig;

	UPROPERTY()
	FGoapWorldState Precondition;

	UPROPERTY(EditAnywhere)
	FGoapWorldStateValueConfig GoalValueConfig;

	UPROPERTY()
	FGoapWorldState Goal;

	UPROPERTY(EditAnywhere)
	bool bCanInterruptGoal;

	UPROPERTY(EditAnywhere, meta=(EditCondition="bCanInterruptGoal", EditConditionHides))
	TArray<FGoapWorldStateValueConfig> InterruptGoalValueConfig;

	UPROPERTY(EditAnywhere)
	FGameplayTag GoapGoalCanUsedActionType;

	UFUNCTION()
	EGoapGoalResult GetGoapGoalResult() { return GoapGoalResult; };

	UFUNCTION()
	void SetGoapGoalResult(const EGoapGoalResult inGoapGoalResult);

	virtual void OnGoalActive(UGoapAgentComponent* GoapAgent);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Goal Active"))
	void ReciveOnGoalActive(UGoapAgentComponent* GoapAgent, AAIController* InAIController, APawn* InControlledPawn);

	virtual void AgentGoapStateChange(const FGoapWorldState& PreGoapWorldState, const FGoapWorldState& CurGoapWorldState);
	//UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "AgentGoapStateChange"))
	//void ReceiveAgentGoapStateChange(const FGoapWorldState& PreGoapWorldState, const FGoapWorldState& CurGoapWorldState);

#if WITH_EDITOR
	void BuildGoapWorldStateData();

	virtual void PostLoad() override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

	void InitGoapState();

	void InitControlerData(AAIController* InAIController);
private:
	EGoapGoalResult GoapGoalResult = EGoapGoalResult::Invalid;
	AAIController* AIController;
	APawn* ControlledPawn;
};