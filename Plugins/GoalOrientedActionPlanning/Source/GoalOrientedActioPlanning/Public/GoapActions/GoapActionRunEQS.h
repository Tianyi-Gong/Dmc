#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GoapAction.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GoapActionRunEQS.generated.h"

UCLASS(config = Game, BlueprintType, Blueprintable)
class GOALORIENTEDACTIONPLANNING_API UGoapActionRunEQS : public UGoapAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UEnvQuery* EnvQuery;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EEnvQueryRunMode::Type> RunMode;

	//UPROPERTY(BlueprintAssignable)
	FQueryFinishedSignature OnQueryFinished;

	virtual void OnActionActive() override;

	void FinishedEQS(TSharedPtr<FEnvQueryResult> Result);

	UFUNCTION(BlueprintImplementableEvent)
	void OnFinishedEQS(const FVector Location);
};

