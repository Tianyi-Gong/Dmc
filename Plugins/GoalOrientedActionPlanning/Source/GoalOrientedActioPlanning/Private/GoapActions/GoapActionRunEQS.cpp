#include "GoapActions\GoapActionRunEQS.h"

void UGoapActionRunEQS::OnActionActive()
{
	Super::OnActionActive();

	if(EnvQuery)
	{
		OnQueryFinished.BindUObject(this,&UGoapActionRunEQS::FinishedEQS);

		UEnvQueryManager* EQS = UEnvQueryManager::GetCurrent(GWorld);

		if(EQS)
		{
			EQS->RunQuery(FEnvQueryRequest(EnvQuery, ControlledPawn), RunMode, OnQueryFinished);
		}
	}
}

void UGoapActionRunEQS::FinishedEQS(TSharedPtr<FEnvQueryResult> Result)
{
	OnFinishedEQS(Result->GetItemAsLocation(0));
}
