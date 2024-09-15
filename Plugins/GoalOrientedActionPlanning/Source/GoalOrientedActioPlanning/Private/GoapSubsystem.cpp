#include "GoapSubsystem.h"

UGoapSubsystem* UGoapSubsystem::GetGoapSubsystem(const UWorld* InWorld)
{
	if (InWorld)
	{
		return InWorld->GetSubsystem<UGoapSubsystem>();
	}

	return nullptr;
}

void UGoapSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GoapDebugController = NewObject<UGoapDebugController>();
	GoapDebugController->Initialize();
}

void UGoapSubsystem::Deinitialize()
{
	Super::Deinitialize();

	GoapDebugController = nullptr;
}

void UGoapSubsystem::WriteGoapWorldData(const FGameplayTag& Key, const bool Value)
{
	CacheGoapStatData.Add(Key, Value);

	CacheGoapStateDataVersion++;
}

void UGoapSubsystem::ClearGoapWorldData()
{
	CacheGoapStatData.Empty();
}

void UGoapSubsystem::GetGoapWorldState(FGoapWorldState& outGoapWorldState)
{

}

const FGoapWorldState& UGoapSubsystem::GetGoapWorldState(const TSubclassOf<UGoapWorldStateRuntimeSettings>& GoapWorldStateRuntimeSettingsClass)
{
	if(FCacheGoapStatFomatData* CacheGoapStatFomatDataPtr = CacheGoapStatFomat.Find(GoapWorldStateRuntimeSettingsClass))
	{
		if(CacheGoapStatFomatDataPtr->GoapStateDataVersion != CacheGoapStateDataVersion)
		{
			CacheGoapStatFomatDataPtr->GoapWorldState.Clear();

			for (auto& [key, value] : CacheGoapStatData)
			{
				CacheGoapStatFomatDataPtr->GoapWorldState.SetStateValue(key, value);
			}
			
			CacheGoapStatFomatDataPtr->GoapStateDataVersion = CacheGoapStateDataVersion;
		}

		return CacheGoapStatFomatDataPtr->GoapWorldState;
	}
	else
	{
		FCacheGoapStatFomatData CacheGoapStatFomatData;
		CacheGoapStatFomatData.GoapWorldState.InitaData(GoapWorldStateRuntimeSettingsClass);
		
		for (auto& [key, value] : CacheGoapStatData)
		{
			CacheGoapStatFomatData.GoapWorldState.SetStateValue(key, value);
		}

		CacheGoapStatFomatData.GoapStateDataVersion = CacheGoapStateDataVersion;

		CacheGoapStatFomat.Add(GoapWorldStateRuntimeSettingsClass, CacheGoapStatFomatData);

		return CacheGoapStatFomat.Find(GoapWorldStateRuntimeSettingsClass)->GoapWorldState;
	}
}

int UGoapSubsystem::GoapWorldStateToStringRawCount()
{
	return CacheGoapStatData.Num();
	//return GoapWorldState.StringRawCount();
}

void UGoapSubsystem::WorldShardStateAppllyEffect(const FGoapWorldState& Effect)
{
	bool bChangeCache = false;
	for (auto&[key,value] : Effect.NamesTable)
	{
		int64 mask = 1i64 << value;
		int worldShard = mask & Effect.SharedFlag;
		int UsedFlag = mask & Effect.NotUsedFlag;
		if( (mask & Effect.SharedFlag) != 0 && 
			(mask & Effect.NotUsedFlag) == 0 )
		{
			bool CacheStateValue = !!(mask & Effect.Values);

			bool bCanWriteCacheData = !CacheGoapStatData.Contains(key) || (CacheGoapStatData.FindRef(key) != CacheStateValue);

			if(bCanWriteCacheData)
			{
				bChangeCache = true;
				CacheGoapStatData.Add(key, CacheStateValue);
			}
		}
	}

	if(bChangeCache)
	{
		CacheGoapStateDataVersion++;
	}
}

FString UGoapSubsystem::GoapWorldStateToString()
{
	FString OutString;

	for (auto& [Key, Value] : CacheGoapStatData)
	{
		OutString += "  " + Key.ToString() + " : " + (Value ? "True \n" : "Fales \n");
	}

	return OutString;
}

void UGoapSubsystem::BindDebugInput(APawn* PlayerPawn)
{
	GoapDebugController->BindDebugInput(PlayerPawn);
}
