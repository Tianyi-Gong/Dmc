#pragma once 
#include "GoapAction.h"
#include "GoapAstarNode.h"
#include "Runtime\AIModule\Classes\AIController.h"
#include "GoapActionSet.generated.h"

UCLASS(config = Game, BlueprintType, Blueprintable)
class GOALORIENTEDACTIONPLANNING_API UGoapActionSet : public UObject
{
	GENERATED_BODY()
public:
	UGoapActionSet();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGoapWorldStateRuntimeSettings> GoapWorldStateRuntimeSettingClass;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly)
	TArray<UGoapAction*> GoapActionPool;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UGoapAction>> GoapActionConfig;

	UPROPERTY(Transient, VisibleAnywhere)
	TArray<UGoapAction*> CanUsedGoapActionPool;

	UFUNCTION(BlueprintCallable)
	void InitData(AAIController* Controller);

	bool IsDataFullLoaded() {return bIsDtatLoaded;}

	UFUNCTION()
	void UpdateCanUsedGoapActionPool(FGameplayTag CanUsedGoapActionType, const FGoapWorldState& GoapWorldState);

	bool GetPossibleTrans(FGoapAstarNode* CurNode, FGoapWorldState& From, FGoapWorldState& To, TArray<FGoapAstarNode *>& outGoapActionArray);

	int CacheCanUsedGoapActionPoolToString(FString& outString);
protected:
	bool bIsDtatLoaded = false;
	AAIController* AIController;
};
