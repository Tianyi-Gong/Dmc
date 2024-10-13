#pragma once 
#include "CoreMinimal.h"
#include "GoapGoal.h"
#include "GoapWorldState.h"
#include "Runtime\AIModule\Classes\AIController.h"
#include "GoapGoalSet.generated.h"

USTRUCT(BlueprintType)
struct GOALORIENTEDACTIONPLANNING_API FGoapGoalConfigData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGoapGoal> GoapGoalClass;

	UPROPERTY(VisibleAnywhere)
	bool bCanConfigWeight;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCanConfigWeight", EditConditionHides ))
	int Weight = 1;
};

USTRUCT(BlueprintType)
struct GOALORIENTEDACTIONPLANNING_API FGoapGoalConfigGroupData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FGoapGoalConfigData> GoapGoalConfigDataList;

	UPROPERTY(EditAnywhere)
	int Index;

	UPROPERTY()
	int Priority;

	UPROPERTY(EditAnywhere)
	bool bRandBeforSelectGoal;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bRandBeforSelectGoal", EditConditionHides))
	bool bRandByWeight;
};

USTRUCT(BlueprintType)
struct GOALORIENTEDACTIONPLANNING_API FGoapGoalSubGroupData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	UGoapGoal* GoapGoalData;

	int Weight;
};

USTRUCT(BlueprintType)
struct GOALORIENTEDACTIONPLANNING_API FGoapGoalSubGroup
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	TArray<FGoapGoalSubGroupData> GoapGoalSupGroup;

	UPROPERTY()
	bool bRandBeforSelectGoal;

	UPROPERTY()
	bool bRandByWeight;

	void ShuffleGroup();
};


UCLASS(config = Game, BlueprintType, Blueprintable)
class GOALORIENTEDACTIONPLANNING_API UGoapGoalSet : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGoapWorldStateRuntimeSettings> GoapWorldStateRuntimeSettingClass;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly)
	TArray<FGoapGoalSubGroup> GoapGoalPool;

	UPROPERTY(EditAnywhere)
	TArray<FGoapGoalConfigGroupData> GoapGoalConfig;

	UFUNCTION(BlueprintCallable)
	void InitData(AAIController* Controller);

	bool IsDataFullLoaded() { return bIsDataLoaded; }

	UGoapGoal* SelectGoalByWorldState(const FGoapWorldState& GoapWorldState);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	bool bIsDataLoaded;
	AAIController* AIController;
};

