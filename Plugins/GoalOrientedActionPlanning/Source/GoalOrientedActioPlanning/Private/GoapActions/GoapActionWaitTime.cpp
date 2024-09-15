#include "GoapActions\GoapActionWaitTime.h"


void UGoapActionWaitTime::OnActionActive()
{
	Super::OnActionActive();

	GWorld->GetTimerManager().SetTimer( WaitTimeTimerHandle,
		                                FTimerDelegate::CreateLambda([this](){
											EndAction(EGoapActionResult::Succeeded);
		                                }),
		                                WaitTime,
										false );
}

void UGoapActionWaitTime::OnActionAborted()
{
	Super::OnActionAborted();

	GWorld->GetTimerManager().ClearTimer(WaitTimeTimerHandle);
}

void UGoapActionWaitTime::OnActionFailed()
{
	Super::OnActionFailed();

	GWorld->GetTimerManager().ClearTimer(WaitTimeTimerHandle);
}
