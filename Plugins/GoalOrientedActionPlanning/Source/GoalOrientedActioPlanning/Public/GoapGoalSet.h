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

	UPROPERTY(EditAnywhere)
	int Index;

	UPROPERTY(VisibleAnywhere)
	int Priority;
};

UCLASS(config = Game, BlueprintType, Blueprintable)
class GOALORIENTEDACTIONPLANNING_API UGoapGoalSet : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGoapWorldStateRuntimeSettings> GoapWorldStateRuntimeSettingClass;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly)
	TArray<UGoapGoal*> GoapGoalPool;

	UPROPERTY(EditAnywhere)
	TArray<FGoapGoalConfigData> GoapGoalConfig;

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

