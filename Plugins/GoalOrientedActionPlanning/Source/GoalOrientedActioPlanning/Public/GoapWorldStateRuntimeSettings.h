#pragma once 

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GoapWorldStateRuntimeSettings.generated.h"

USTRUCT()
struct GOALORIENTEDACTIONPLANNING_API FGoapWorldStateConfigData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere)
	bool bIsWorldShared;
};

USTRUCT()
struct GOALORIENTEDACTIONPLANNING_API FGoapWorldStateSettingData
{
	GENERATED_USTRUCT_BODY()
public:
	FGoapWorldStateSettingData();
	
	UPROPERTY(VisibleAnywhere)
	int64 SharedFlag;

	UPROPERTY(VisibleAnywhere)
	TMap<FGameplayTag, int> NamesTable;

	UPROPERTY(VisibleAnywhere)
	int curNamsLen;

	void Clear();
	bool RebuildGoapWorldStateSettingData(TMap<FGameplayTag, FGoapWorldStateConfigData> BuildData);
};


UCLASS(config=Engine, defaultconfig, BlueprintType, Blueprintable)
class GOALORIENTEDACTIONPLANNING_API UGoapWorldStateRuntimeSettings : public UObject
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, config, Category=Setings)
	TMap<FGameplayTag, FGoapWorldStateConfigData> GoapWorldStateRuntimeSettings;

	UPROPERTY(VisibleAnywhere, config, Category=Setings)
	FGoapWorldStateSettingData GoapWorldStateSettingData;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
