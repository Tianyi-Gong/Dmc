// Fill out your copyright notice in the Description page of Project Settings.


#include "GoapDebugUserWidget.h"

void UGoapDebugUserWidget::OnStartDebugAI(UGoapAgentComponent* AIComponent)
{
	DebugAIComponent = AIComponent;

	ReceiveOnStartDebugAI(AIComponent);
}
