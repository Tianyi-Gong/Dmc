// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GoapAgentComponent.h"
#include "GoapDebugUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class GOALORIENTEDACTIONPLANNING_API UGoapDebugUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void OnStartDebugAI(UGoapAgentComponent* AIComponent);
	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveOnStartDebugAI(const UGoapAgentComponent* AIComponent);
	
	UPROPERTY(BlueprintReadOnly)
	UGoapAgentComponent* DebugAIComponent;
};
