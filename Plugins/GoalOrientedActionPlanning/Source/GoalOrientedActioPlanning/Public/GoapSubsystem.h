#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GoapWorldState.h"
#include "GameplayTagContainer.h"
#include "Debug/GoapDebugController.h"
#include "GoapSubsystem.generated.h"

USTRUCT()
struct GOALORIENTEDACTIONPLANNING_API FCacheGoapStatFomatData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	FGoapWorldState GoapWorldState;

	UPROPERTY()
	int64 GoapStateDataVersion;
};

UCLASS()
class GOALORIENTEDACTIONPLANNING_API UGoapSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UGoapSubsystem* GetGoapSubsystem(const UWorld* InWorld);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void WriteGoapWorldData(const FGameplayTag& Key,const bool Value);

	UFUNCTION(BlueprintCallable)
	void ClearGoapWorldData();

	UFUNCTION(BlueprintCallable)
	void GetGoapWorldState(FGoapWorldState& outGoapWorldState);

	UFUNCTION(BlueprintCallable)
	FString GoapWorldStateToString();

	UFUNCTION(BlueprintCallable)
	void BindDebugInput(APawn* PlayerPawn);

	const FGoapWorldState& GetGoapWorldState(const TSubclassOf<UGoapWorldStateRuntimeSettings>& GoapWorldStateRuntimeSettingsClass);

	//UPROPERTY();
	//FGoapWorldState GoapWorldState;

	int GoapWorldStateToStringRawCount();

	void WorldShardStateAppllyEffect(const FGoapWorldState& Effect);
private:
	UPROPERTY();
	UGoapDebugController* GoapDebugController;

	TMap<TSubclassOf<UGoapWorldStateRuntimeSettings>, FCacheGoapStatFomatData> CacheGoapStatFomat;
	TMap<FGameplayTag,bool> CacheGoapStatData;
	int64 CacheGoapStateDataVersion;
};

