#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GoapAction.h"
#include "Abilities/GameplayAbility.h"
#include "GameAbilitySystem\GoapAbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GoapActionAbility.generated.h"

UCLASS(config = Game, BlueprintType, Blueprintable)
class GOALORIENTEDACTIONPLANNING_API UGoapActionAbility : public UGoapAction
{
	GENERATED_BODY()
	~UGoapActionAbility();
public:
	UPROPERTY(EditAnywhere)
	TArray<FAbilityCDConfig> AbilityConfigList;

	UPROPERTY(EditAnywhere)
	bool bUpdateCostByAbilityGroupCDInfo;

	UPROPERTY(EditAnywhere, meta=(EditCondition="bUpdateCostByAbilityGroupCDInfo", EditConditionHides))
	FGameplayTag CheckAbilityGroupGameplayTag;

	UFUNCTION(BlueprintCallable)
	void MotionWarpingToTarget(FName MotionWarpingTage, FName TargetActorKey);

	UFUNCTION(BlueprintCallable)
	void ActiveAbilityByAbilityGroupTag(FGameplayTag GameplayTag,bool& bResult, FGameplayAbilitySpecHandle& GameplayAbilitySpecHandle);

	virtual void InitControlerData(AAIController* InAIController) override;
	void InitAbilityData();
	void UnInitAbilityData();

	void OnAbilityEndDelegate(const FAbilityEndedData& AbilityEndedData);

	virtual void UpdateDynamicCostByGoapState(const FGoapWorldState& GoapWorldState) override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnAbilityEndDelegate"))
	void ReciveOnAbilityEndDelegate(const FGameplayAbilitySpecHandle& GameplayAbilitySpecHandle, bool bWasCancelled);
private:
	FDelegateHandle AbilityEndDelegateHandle;
	UGoapAbilitySystemComponent* GoapAbilitySystemComponent;
	FGameplayAbilitySpecHandle ActiveGameplayAbilitySpecHandle;
};

