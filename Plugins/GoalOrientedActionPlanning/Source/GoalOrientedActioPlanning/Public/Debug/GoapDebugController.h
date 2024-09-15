// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Canvas.h"
#include "GameplayDebuggerTypes.h"

#include "GoapAgentComponent.h"

#include "GoapDebugController.generated.h"

/**
 * 
 */
UCLASS()
class GOALORIENTEDACTIONPLANNING_API UGoapDebugController : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	UFont* HUDFont;

	virtual void BeginPlay();

	void Initialize();

#if WITH_GAMEPLAY_DEBUGGER_MENU
	void DrawHeader(FGameplayDebuggerCanvasContext& CanvasContext);
	int DrawGoapState(int DrawRawCount,FGameplayDebuggerCanvasContext& CanvasContext);
	int DrawGoapPlanedAction(int DrawRawCount, FGameplayDebuggerCanvasContext& CanvasContext);

	void OnDebugDraw(class UCanvas* Canvas, class APlayerController* PC);
	FString GetColorStringByIndex(int Index);
#endif
	void ToggleActivation();
	void ToggleDebugSoltToOne();
	void ToggleDebugSoltToTwo();
	void BindDebugInput(APawn* PlayerPawn);
	void UnbindDebugInput();
protected:
#if WITH_EDITOR
	void OnSelectionChanged(UObject* Object);
	void OnSelectedObject(UObject* Object);
#endif
	APawn* DebugInputPawn;
	APawn* DebugOwnerActor;
	UGoapAgentComponent* DebugGoapAgentComponent;
	bool bDebugDrawEnabled;

	float PaddingLeft;
	float PaddingRight;
	float PaddingTop;
	float PaddingBottom;

	int DebugIndex;
};
