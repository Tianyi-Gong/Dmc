#include "GoapWorldStateRuntimeSettings.h"

#define INT64MAX 0xFFFFFFFFFFFFFFFF
#define MAXATOMS 64

FGoapWorldStateSettingData::FGoapWorldStateSettingData()
{
	Clear();
}

#if WITH_EDITOR
void UGoapWorldStateRuntimeSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if(PropertyChangedEvent.Property != nullptr)
	{
		const FName PropertyName(PropertyChangedEvent.MemberProperty->GetFName());
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UGoapWorldStateRuntimeSettings, GoapWorldStateRuntimeSettings))
		{
			GoapWorldStateSettingData.RebuildGoapWorldStateSettingData(GoapWorldStateRuntimeSettings);

			FCoreUObjectDelegates::OnObjectPropertyChanged.Broadcast(this, PropertyChangedEvent);
		}
	}
}
#endif

void FGoapWorldStateSettingData::Clear()
{
	SharedFlag = INT64MAX;
	NamesTable.Empty();
	curNamsLen = 0;
}

bool FGoapWorldStateSettingData::RebuildGoapWorldStateSettingData(TMap<FGameplayTag, FGoapWorldStateConfigData> BuildData)
{
	Clear();

	for (auto&[Key,Value] : BuildData)
	{
		int64 mask = 1i64 << curNamsLen;

		NamesTable.Add(Key,curNamsLen);

		if(!Value.bIsWorldShared)
		{
			SharedFlag &= ~mask;
		}

		curNamsLen++;
	}

	return true;
}
