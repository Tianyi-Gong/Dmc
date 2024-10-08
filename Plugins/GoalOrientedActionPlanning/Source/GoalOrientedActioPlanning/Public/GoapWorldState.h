#pragma once

#include "CoreMinimal.h"
#include "GoapWorldStateRuntimeSettings.h"
#include "GoapWorldState.generated.h"

USTRUCT(BlueprintType)
struct GOALORIENTEDACTIONPLANNING_API FGoapWorldStateValueConfigData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
	bool bStatValue;

	UPROPERTY(VisibleAnywhere)
	bool bIsWorldShared;
};


USTRUCT(BlueprintType)
struct GOALORIENTEDACTIONPLANNING_API FGoapWorldStateValueConfig
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FGoapWorldStateValueConfigData> StateValueConfig;
};

USTRUCT(BlueprintType)
struct GOALORIENTEDACTIONPLANNING_API FGoapWorldState
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGoapWorldStateRuntimeSettings> GoapWorldStateRuntimeSettingClass;

	UPROPERTY(VisibleAnywhere)
	int64 Values;

	UPROPERTY(VisibleAnywhere)
	int64 NotUsedFlag;

	UPROPERTY(VisibleAnywhere)
	int64 SharedFlag;

	UPROPERTY(VisibleAnywhere)
	TMap<FGameplayTag,int> NamesTable;

	UPROPERTY(VisibleAnywhere)
	int curNamsLen;

	FGoapWorldState();

	~FGoapWorldState();

	void InitaData(TSubclassOf<UGoapWorldStateRuntimeSettings> InGoapWorldStateRuntimeSettingClass);

	bool SetStateValue(FGameplayTag StatName, bool StateValue = false);
	bool GetStateValue(FGameplayTag StatName, bool& StateValue, bool IgnoreUsedFlag = false);

	int CalcCorrelation(const FGoapWorldState& to);

	void SetValues(int64 inValues);
	void SetNotUsedFlag(int64 inNotUsedFlag);

	void Clear();

	int FindOrAddStatName(FGameplayTag StatName);

	FString ToString();
	int StringRawCount();

#if WITH_EDITOR
	void BuildValueByGoapWorldStateValueConfig(FGoapWorldStateValueConfig* config);
	void OnPropertyChanged(UObject* ObjectBeingModified, FPropertyChangedEvent& PropertyChangedEvent);
	void UnbindFunction();
#endif // WITH_EDITOR

	bool operator !=(const FGoapWorldState& GoapWorldState)
	{
		return (this->Values & ~this->NotUsedFlag) != (GoapWorldState.Values & ~GoapWorldState.NotUsedFlag);
	}
private:
#if WITH_EDITOR
	FDelegateHandle OnPropertyChangedDelegateHandle;
	FGoapWorldStateValueConfig* GoapWorldStateValueConfig;
#endif
	UPROPERTY()
	TMap<FString,bool> ValueMapData;
	UPROPERTY()
	TMap<FString,bool> NotUsedFlagMapData;
};