// Fill out your copyright notice in the Description page of Project Settings.


#include "Debug/GoapDebugController.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Selection.h"
#include "EngineUtils.h"
#include "GameFramework/SpectatorPawn.h"
#include "GameplayDebuggerConfig.h"
#include "GoapSubsystem.h"

void UGoapDebugController::BeginPlay()
{
	Initialize();
}

void UGoapDebugController::Initialize()
{
#if WITH_GAMEPLAY_DEBUGGER_MENU
	UDebugDrawService::Register(TEXT("Game"), FDebugDrawDelegate::CreateUObject(this, &UGoapDebugController::OnDebugDraw));

#if WITH_EDITOR
	if (GIsEditor)
	{
		//USelection::SelectionChangedEvent.AddUObject(this, &UGoapDebugController::OnSelectionChanged);
		//USelection::SelectObjectEvent.AddUObject(this, &UGoapDebugController::OnSelectedObject);
	}
#endif

	const UGameplayDebuggerConfig* SettingsCDO = UGameplayDebuggerConfig::StaticClass()->GetDefaultObject<UGameplayDebuggerConfig>();
	PaddingLeft = SettingsCDO->DebugCanvasPaddingLeft;
	PaddingRight = SettingsCDO->DebugCanvasPaddingRight;
	PaddingTop = SettingsCDO->DebugCanvasPaddingTop;
	PaddingBottom = SettingsCDO->DebugCanvasPaddingBottom;

	HUDFont = NewObject<UFont>(this, TEXT("HUDFont"), RF_NoFlags, GEngine->GetSmallFont());
	HUDFont->LegacyFontSize = UGameplayDebuggerUserSettings::GetFontSize();
#endif
}

extern RENDERCORE_API FTexture* GWhiteTexture;

#if WITH_GAMEPLAY_DEBUGGER_MENU
void UGoapDebugController::DrawHeader( FGameplayDebuggerCanvasContext& CanvasContext)
{
	const float LineHeight = CanvasContext.GetLineHeight();
	const float DPIScale = CanvasContext.Canvas->GetDPIScale();
	const float CanvasSizeX = (CanvasContext.Canvas->SizeX / DPIScale) - PaddingLeft - PaddingRight;
	const float UsePaddingTop = PaddingTop;

	const float BackgroundPadding = 5.0f;
	const float BackgroundPaddingBothSides = BackgroundPadding * 2.0f;

	FCanvasTileItem TileItem(FVector2D(0, 0), GWhiteTexture, FVector2D(CanvasSizeX - BackgroundPadding, LineHeight * 3) + BackgroundPaddingBothSides, FLinearColor(0, 0, 0, 0.2f));
	TileItem.BlendMode = SE_BLEND_Translucent;
	CanvasContext.DrawItem(TileItem, PaddingLeft - BackgroundPadding , UsePaddingTop - BackgroundPadding);
	
	CanvasContext.Printf(TEXT("Current GoapAI instance: {yellow}%s"), *(DebugOwnerActor->GetActorNameOrLabel()));
	CanvasContext.Printf(TEXT("Press num switch debug option."));
	CanvasContext.Printf(TEXT("  %s1.GoapState %s2.PlanAction"), *GetColorStringByIndex(0), *GetColorStringByIndex(1));

	if(DebugGoapAgentComponent != nullptr)
	{
		CanvasContext.PrintfAt(BackgroundPadding * 100, UsePaddingTop + LineHeight * 0.5, TEXT("Agent Goal: {yellow}%s"), (DebugGoapAgentComponent->CurGoapGoal != nullptr) ? *(DebugGoapAgentComponent->CurGoapGoal->GetClass()->GetName()) : *FString("None"));
		CanvasContext.PrintfAt(BackgroundPadding * 100, UsePaddingTop + LineHeight * 1.5, TEXT("Agent Action: {yellow}%s"), (DebugGoapAgentComponent->CurActiveAction != nullptr) ? *(DebugGoapAgentComponent->CurActiveAction->GetClass()->GetName()) : *FString("None"));
	}
}


void UGoapDebugController::OnDebugDraw(class UCanvas* Canvas, class APlayerController* PC)
{
	const FSceneInterface* Scene = (Canvas && Canvas->Canvas) ? Canvas->Canvas->GetScene() : nullptr;
	if(Scene && Scene->GetWorld() != GWorld)
	{
		return;
	}

	if(DebugOwnerActor != nullptr && bDebugDrawEnabled)
	{
		FGameplayDebuggerCanvasContext CanvasContext(Canvas, HUDFont);
		CanvasContext.CursorX = CanvasContext.DefaultX = PaddingLeft;
		CanvasContext.CursorY = CanvasContext.DefaultY = PaddingTop;

		CanvasContext.FontRenderInfo.bEnableShadow = false;

		CanvasContext.PlayerController = GWorld->GetFirstPlayerController();
		CanvasContext.World = GWorld;

		DrawHeader(CanvasContext);

		switch (DebugIndex)
		{
		case 1:
			DrawGoapPlanedAction(4,CanvasContext);
			break;
		case 0:
		default:
			int RawCount = DrawGoapState(4,CanvasContext);
			DrawGoapPlanedAction(RawCount + 2,CanvasContext);
			break;
		}
	}
	else
	{

	}
}

FString UGoapDebugController::GetColorStringByIndex(int Index)
{
	if(Index == DebugIndex)
	{
		return "{yellow}";
	}
	return "{white}";
}

int UGoapDebugController::DrawGoapState(int DrawRawCount, FGameplayDebuggerCanvasContext& CanvasContext)
{
	if (DebugGoapAgentComponent == nullptr)
		return 0;

	const float LineHeight = CanvasContext.GetLineHeight();
	const float UsePaddingTop = PaddingTop;

	const float BackgroundPadding = 5.0f;

	int LineHeightScale = DrawRawCount;
	CanvasContext.PrintfAt(BackgroundPadding * 2, UsePaddingTop + LineHeight * LineHeightScale, TEXT("Agent State:\n%s"), *(DebugGoapAgentComponent->GoapPrivateState.ToString()));
	
	UGoapSubsystem* GoapSubsystem = GWorld->GetSubsystem<UGoapSubsystem>();
	CanvasContext.PrintfAt(BackgroundPadding * 50, UsePaddingTop + LineHeight * LineHeightScale, TEXT("World State:\n%s"), (GoapSubsystem != nullptr) ? *(GoapSubsystem->GoapWorldStateToString()) : *FString("None"));

	UGoapGoal* CurrentGoal = DebugGoapAgentComponent->CurGoapGoal;

	CanvasContext.PrintfAt(BackgroundPadding * 100, UsePaddingTop + LineHeight * LineHeightScale, TEXT("Goal State:\n%s"), CurrentGoal != nullptr ? *(CurrentGoal->Goal.ToString()) : *FString("None"));
	int CurrentGoalLength = CurrentGoal != nullptr ? CurrentGoal->Goal.StringRawCount() : 1;


	int MaxLength = FMath::Max3(DebugGoapAgentComponent->GoapPrivateState.StringRawCount(), GoapSubsystem->GoapWorldStateToStringRawCount(), CurrentGoalLength);

	LineHeightScale += MaxLength;

	return LineHeightScale;
}

int UGoapDebugController::DrawGoapPlanedAction(int DrawRawCount, FGameplayDebuggerCanvasContext& CanvasContext)
{
	if(DebugGoapAgentComponent == nullptr)
		return 0;

	const float LineHeight = CanvasContext.GetLineHeight();
	const float UsePaddingTop = PaddingTop;

	const float BackgroundPadding = 5.0f;
	int LineHeightScale = DrawRawCount;

	CanvasContext.PrintfAt(BackgroundPadding * 2, UsePaddingTop + LineHeight * LineHeightScale,TEXT("Goal can used ActionType: {yellow}%s"), (DebugGoapAgentComponent->CurGoapGoal != nullptr) ? *(DebugGoapAgentComponent->CurGoapGoal->GoapGoalCanUsedActionType.ToString()) : *FString("Nullptr"));

	FString GoalCanUsedActionListString;
	int PoolLength = DebugGoapAgentComponent->GoapActionSet->CacheCanUsedGoapActionPoolToString(GoalCanUsedActionListString);

	LineHeightScale++;
	CanvasContext.PrintfAt(BackgroundPadding * 2, UsePaddingTop + LineHeight * LineHeightScale,TEXT("Goal can used ActionList:\n%s"), *GoalCanUsedActionListString);

	LineHeightScale = LineHeightScale + PoolLength + 1;
	CanvasContext.PrintfAt(BackgroundPadding * 2, UsePaddingTop + LineHeight * LineHeightScale, TEXT("Goal Planed Action:"));

	LineHeightScale++;
	CanvasContext.PrintfAt(BackgroundPadding * 2, UsePaddingTop + LineHeight * LineHeightScale, TEXT("%s"), !DebugGoapAgentComponent->GoapAstarPlanner->PlannedAction.IsEmpty() ? *(DebugGoapAgentComponent->GoapAstarPlanner->GetPlannedActionData(DebugGoapAgentComponent->CurActiveAction)) : *FString("None"));

	return LineHeightScale;
}

#endif

void UGoapDebugController::ToggleActivation()
{
	if(!bDebugDrawEnabled)
	{
		APlayerController* OwnerPC = GWorld->GetFirstPlayerController();
		if(OwnerPC)
		{
			FVector ViewLocation = OwnerPC->PlayerCameraManager->GetCameraLocation();
			FVector ViewDirection = OwnerPC->PlayerCameraManager->GetCameraRotation().Vector();

			const UGameplayDebuggerUserSettings* Settings = GetDefault<UGameplayDebuggerUserSettings>();
			const FVector::FReal MaxScanDistance = Settings->MaxViewDistance;
			const FVector::FReal MinViewDirDot = FMath::Cos(FMath::DegreesToRadians(Settings->MaxViewAngle));

			APawn* BestCandidate = nullptr;
			FVector::FReal BestScore = MinViewDirDot;

			for (APawn* TestPawn : TActorRange<APawn>(OwnerPC->GetWorld()))
			{
				if (!TestPawn->IsHidden() && TestPawn->GetActorEnableCollision() &&
					!TestPawn->IsA(ASpectatorPawn::StaticClass()) &&
					TestPawn != OwnerPC->GetPawn())
				{
					FVector DirToPawn = (TestPawn->GetActorLocation() - ViewLocation);
					FVector::FReal DistToPawn = DirToPawn.Size();
					if (FMath::IsNearlyZero(DistToPawn))
					{
						DirToPawn = ViewDirection;
						DistToPawn = 1.;
					}
					else
					{
						DirToPawn /= DistToPawn;
					}

					const FVector::FReal ViewDot = FVector::DotProduct(ViewDirection, DirToPawn);
					if (DistToPawn < MaxScanDistance && ViewDot > BestScore)
					{
						BestScore = ViewDot;
						BestCandidate = TestPawn;
					}
				}
			}

			if (DebugOwnerActor != BestCandidate)
			{
				DebugOwnerActor = BestCandidate;

				if (UGoapAgentComponent* GoapAgentComponent = DebugOwnerActor->GetController()->GetComponentByClass<UGoapAgentComponent>())
				{
					DebugGoapAgentComponent = GoapAgentComponent;
				}
			}
		}

		if(DebugOwnerActor)
		{
			bDebugDrawEnabled = true;
		}
	}
	else
	{
		DebugGoapAgentComponent = nullptr;
		DebugOwnerActor = nullptr;

		bDebugDrawEnabled = false;
	}
	
}

void UGoapDebugController::ToggleDebugSoltToOne()
{
	if(bDebugDrawEnabled)
	{
		DebugIndex = 0;
	}
}

void UGoapDebugController::ToggleDebugSoltToTwo()
{
	if (bDebugDrawEnabled)
	{
		DebugIndex = 1;
	}
}

void UGoapDebugController::BindDebugInput(APawn* PlayerPawn)
{
	if(DebugOwnerActor != nullptr)
		return;

	DebugOwnerActor = PlayerPawn;

	DebugOwnerActor->InputComponent->BindKey(EKeys::P, EInputEvent::IE_Pressed, this, &UGoapDebugController::ToggleActivation);
	DebugOwnerActor->InputComponent->BindKey(EKeys::One, EInputEvent::IE_Pressed, this, &UGoapDebugController::ToggleDebugSoltToOne);
	DebugOwnerActor->InputComponent->BindKey(EKeys::Two, EInputEvent::IE_Pressed, this, &UGoapDebugController::ToggleDebugSoltToTwo);
}

void UGoapDebugController::UnbindDebugInput()
{
	if(DebugOwnerActor == nullptr)
		return;

	
	DebugOwnerActor = nullptr;
}

#if WITH_EDITOR
void UGoapDebugController::OnSelectionChanged(UObject* Object)
{
	if (Object && Object->GetWorld() != GetWorld())
	{
		return;
	}

	AController* SelectedController = Cast<AController>(Object);
	APawn* SelectedPawn = SelectedController ? SelectedController->GetPawn() : Cast<APawn>(Object);
	if (SelectedPawn && SelectedPawn->IsSelected())
	{
		if (UGoapAgentComponent* GoapAgentComponent = SelectedPawn->GetComponentByClass<UGoapAgentComponent>())
		{
			DebugGoapAgentComponent = GoapAgentComponent;
		}
	}
}

void UGoapDebugController::OnSelectedObject(UObject* Object)
{
	if (Object && Object->GetWorld() != GetWorld())
	{
		return;
	}

	AController* SelectedController = Cast<AController>(Object);
	APawn* SelectedPawn = SelectedController ? SelectedController->GetPawn() : Cast<APawn>(Object);
	if(SelectedPawn && SelectedPawn->IsSelected())
	{
		if(UGoapAgentComponent* GoapAgentComponent = SelectedPawn->GetComponentByClass<UGoapAgentComponent>())
		{
			DebugGoapAgentComponent = GoapAgentComponent;
		}
	}
}
#endif