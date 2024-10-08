#include "GoapWorldState.h"

#define INT64MAX 0xFFFFFFFFFFFFFFFF
#define INT64MAXLENGTH 64

int GGoapAStarHValueScale = 1;
static FAutoConsoleVariableRef CVarNiagaraMinQualityLevel(
	TEXT("Goap.GoapAStarHValueScale"),
	GGoapAStarHValueScale,
	TEXT("Controll A Star H Value Scale. \n"),
	ECVF_ReadOnly
);

FGoapWorldState::FGoapWorldState()
{
	//ToDo: 
	/*{
		const FGoapWorldStateSettingData& worldStateDesc = GetDefault<UGoapWorldStateRuntimeSettings>()->GoapWorldStateSettingData;

		NamesTable = TMap<FGameplayTag, int>(worldStateDesc.NamesTable);

		curNamsLen = worldStateDesc.curNamsLen;
		SharedFlag = worldStateDesc.SharedFlag;
	}*/
	InitaData(nullptr);

	Values = 0;
	NotUsedFlag = INT64MAX;

#if WITH_EDITOR
	//OnPropertyChangedDelegateHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this, &FGoapWorldState::OnPropertyChanged);
#endif
}

FGoapWorldState::~FGoapWorldState()
{
#if WITH_EDITOR
	UnbindFunction();
#endif
}

void FGoapWorldState::InitaData(TSubclassOf<UGoapWorldStateRuntimeSettings> InGoapWorldStateRuntimeSettingClass)
{
	if(InGoapWorldStateRuntimeSettingClass.Get() != nullptr)
	{
		GoapWorldStateRuntimeSettingClass = InGoapWorldStateRuntimeSettingClass;

		const FGoapWorldStateSettingData& worldStateDesc = Cast<UGoapWorldStateRuntimeSettings>(GoapWorldStateRuntimeSettingClass->GetDefaultObject(true))->GoapWorldStateSettingData;

		NamesTable = TMap<FGameplayTag, int>(worldStateDesc.NamesTable);

		curNamsLen = worldStateDesc.curNamsLen;
		SharedFlag = worldStateDesc.SharedFlag;
	}
	else
	{
		const FGoapWorldStateSettingData& worldStateDesc = GetDefault<UGoapWorldStateRuntimeSettings>()->GoapWorldStateSettingData;

		NamesTable = TMap<FGameplayTag, int>(worldStateDesc.NamesTable);

		curNamsLen = worldStateDesc.curNamsLen;
		SharedFlag = worldStateDesc.SharedFlag;
	}
}

bool FGoapWorldState::SetStateValue(FGameplayTag StatName, bool StateValue /*= false*/)
{	
	int NotFindResult = -1;

	int Pos = NamesTable.FindRef(StatName, NotFindResult);

	if(Pos == NotFindResult)
		return false;

	int64 mask = 1i64 << Pos;
	Values = StateValue ? (Values | mask) : (Values & ~mask);

	NotUsedFlag &= ~mask;

#if WITH_EDITOR
	ValueMapData.Add(StatName.ToString(), StateValue);
	NotUsedFlagMapData.Add(StatName.ToString(),0);
#endif
	return true;
}

bool FGoapWorldState::GetStateValue(FGameplayTag StatName, bool& StateValue, bool IgnoreUsedFlag)
{
	int NotFindResult = -1;
	int Pos = NamesTable.FindRef(StatName, NotFindResult);

	if (Pos == NotFindResult)
		return false;

	int64 mask = 1i64 << Pos;

	if((NotUsedFlag & mask) != 0 && !IgnoreUsedFlag)
		return false;

	StateValue = !!(Values & mask);

	return true;
}

int FGoapWorldState::CalcCorrelation(const FGoapWorldState& to)
{
	int64 UseFlag = ~to.NotUsedFlag;
	int64 diff = (Values & UseFlag) ^ (to.Values & UseFlag);
	int dist = 0;

	int TableNum = NamesTable.Num();

	for (size_t i = 0; i < TableNum; i++)
	{
		if( (diff & (1i64 << i)) != 0)
		{
			dist++;
		}
	}

	return dist * GGoapAStarHValueScale;
}

void FGoapWorldState::SetValues(int64 inValues)
{
	Values = inValues;

#if WITH_EDITOR
	TArray<FGameplayTag> KeyList;
	NamesTable.GenerateKeyArray(KeyList);

	int64 UsedFlag = ~NotUsedFlag;
	for(int i = 0; i< NamesTable.Num(); i++)
	{
		int64 mask = 1i64 << i;

		if((mask & UsedFlag) != 0)
		{

			bool value = (inValues & mask) != 0;
			ValueMapData.Add(KeyList[i].ToString(),value);
		}
	}
#endif
}

void FGoapWorldState::SetNotUsedFlag(int64 inNotUsedFlag)
{
	NotUsedFlag = inNotUsedFlag;
}

void FGoapWorldState::Clear()
{
	Values = 0;
	NotUsedFlag = INT64MAX;
}

int FGoapWorldState::FindOrAddStatName(FGameplayTag StatName)
{
	if(NamesTable.Contains(StatName))
	{
		return NamesTable.FindRef(StatName);
	}

	if(curNamsLen < INT64MAXLENGTH)
	{
		NamesTable.Add(StatName,curNamsLen);

		curNamsLen++;
		
		return curNamsLen;
	}

	return -1;
}

FString FGoapWorldState::ToString()
{
	if(ValueMapData.Num() == 0)
	{
		return "None";
	}

	FString OutString;

	for (auto& [Key, Value] : ValueMapData)
	{
		OutString += "  " + Key + " : " + (Value ? "True \n" : "Fales \n");
	}

	return OutString;
}

int FGoapWorldState::StringRawCount()
{
	return ValueMapData.Num();
}

#if WITH_EDITOR
void FGoapWorldState::BuildValueByGoapWorldStateValueConfig(FGoapWorldStateValueConfig* config)
{
	if(config == nullptr)
		return;

	GoapWorldStateValueConfig = config;

	Clear();

	for (auto& [key,value] : GoapWorldStateValueConfig->StateValueConfig)
	{
		SetStateValue(key,value.bStatValue);
	}
}


void FGoapWorldState::OnPropertyChanged(UObject* ObjectBeingModified, FPropertyChangedEvent& PropertyChangedEvent)
{	
	const FName PropertyName(PropertyChangedEvent.MemberProperty->GetFName());
	if (PropertyName != GET_MEMBER_NAME_CHECKED(UGoapWorldStateRuntimeSettings, GoapWorldStateRuntimeSettings))
		return;

	if(UGoapWorldStateRuntimeSettings* GoapWorldStateRuntimeSetting = Cast<UGoapWorldStateRuntimeSettings>(ObjectBeingModified))
	{
		NamesTable.Empty();

		const FGoapWorldStateSettingData& worldStateDesc = GetDefault<UGoapWorldStateRuntimeSettings>()->GoapWorldStateSettingData;

		//const FGoapWorldStateSettingData& worldStateDesc = GoapWorldStateRuntimeSetting->GoapWorldStateSettingData;

		//NamesTable = worldStateDesc.NamesTable;

		TArray<FGameplayTag> Keys;
		worldStateDesc.NamesTable.GenerateKeyArray(Keys);

		TArray<int> values;
		worldStateDesc.NamesTable.GenerateValueArray(values);

		for (int i = 0;i < Keys.Num();i++)
		{
			FGameplayTag key = Keys[i];
			int value = values[i];

			NamesTable.Add(key,value);
		}

		Values = 0;
		NotUsedFlag = INT64MAX;

		curNamsLen = worldStateDesc.curNamsLen;
		SharedFlag = worldStateDesc.SharedFlag;

		if(GoapWorldStateValueConfig != nullptr)
		{
			for (auto& [key, value] : GoapWorldStateValueConfig->StateValueConfig)
			{
				SetStateValue(key, value.bStatValue);

				value.bIsWorldShared = GoapWorldStateRuntimeSetting->GoapWorldStateRuntimeSettings.FindRef(key).bIsWorldShared;
			}
		}
	}
}

void FGoapWorldState::UnbindFunction()
{
	if(OnPropertyChangedDelegateHandle.IsValid())
	{
		FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(OnPropertyChangedDelegateHandle);
		OnPropertyChangedDelegateHandle.Reset();
	}
}

#endif