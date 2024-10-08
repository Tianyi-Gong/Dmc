#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "GoapAbilitySystemComponent.generated.h"

USTRUCT(BlueprintType)
struct GOALORIENTEDACTIONPLANNING_API FAbilityCDConfig
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayAbility> PreloadedAbility;

	UPROPERTY(EditAnywhere)
	float CoolDownTime;

	UPROPERTY(EditAnywhere);
	int Weight = 1;

	UPROPERTY(EditAnywhere)
	FGameplayTag AbilityGroup;
};

USTRUCT(BlueprintType)
struct GOALORIENTEDACTIONPLANNING_API FAbilityCDData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
	float CoolDownTime;

	UPROPERTY(EditAnywhere);
	int Weight = 1;

	UPROPERTY()
	FGameplayAbilitySpecHandle GameplayAbilitySpecHandle;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayAbility> PreloadedAbility;

	bool IsCoolDownComplete();

	bool Active();

	int GetCurrentWeight();

	void Tick(float DeltaTime);
private:
	float CurrentCoolDownTime;
};


UCLASS()
class GOALORIENTEDACTIONPLANNING_API UGoapAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	FGameplayAbilitySpecHandle GiveAbilityAndAddToAbilityGroup(const FAbilityCDConfig& AbilityCDConfig);
	void ClearAbilityAndRemoveFormAbilityGroup(const FAbilityCDConfig& AbilityCDConfig);

	FGameplayAbilitySpecHandle TryActiveAbilityByWeightFromAbilityGroup(FGameplayTag AbilityGroup);
	bool GetAbilityGroupInfo(FGameplayTag AbilityGroup,int& CDAbilityCount, int& AbilityCount);

	virtual bool GetShouldTick() const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	TMap<FGameplayTag,TArray<FAbilityCDData>> AbilityGroupDataMap;
};

