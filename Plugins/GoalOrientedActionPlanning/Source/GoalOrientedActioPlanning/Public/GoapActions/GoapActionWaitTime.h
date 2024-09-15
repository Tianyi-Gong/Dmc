#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GoapAction.h"
#include "TimerManager.h"
#include "GoapActionWaitTime.generated.h"

UCLASS(config = Game, BlueprintType, Blueprintable)
class GOALORIENTEDACTIONPLANNING_API UGoapActionWaitTime : public UGoapAction
{
	GENERATED_BODY()
public:
	virtual void OnActionActive();
	virtual void OnActionAborted();
	virtual void OnActionFailed();

	UPROPERTY(EditAnywhere)
	float WaitTime = 1.0f;

private:
	FTimerHandle WaitTimeTimerHandle;
};

