// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AttackFeelComponent.h"

static int GAttackCheckDebugType = 0;
static FAutoConsoleVariableRef CVarAIComboAttackCheckDebugType(
	TEXT("Weapon.AttackCheckDebugType"),
	GAttackCheckDebugType,
	TEXT(" 0=None 1=ForOneFrame 2=ForDuration 3=Persistent 4=OnlyHitPosition")
);

UWeaponMeshComponent::UWeaponMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	DebugHitColor = FLinearColor::MakeRandomColor();
}

void UWeaponMeshComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);

	if (bIsAttackChecking)
	{
		for (int index = 0; index < WeapongAttckSocketList.Num(); index++)
		{
			FVector AttackCheckStart = PreAttackCheckLocation[index];
			FVector AttackCheckEnd = GetSocketLocation(WeapongAttckSocketList[index]);

			FHitResult HitResult;

			int AttackCheckDebugType = GAttackCheckDebugType;
			if(GAttackCheckDebugType > 3 || GAttackCheckDebugType < 0)
			{
				AttackCheckDebugType = 0;
			}

			//bool bFindResult = UKismetSystemLibrary::LineTraceSingleForObjects(GetOwner(), AttackCheckStart, AttackCheckEnd, { EObjectTypeQuery::ObjectTypeQuery3 }, false, AttackCheckIgnoreActor, (EDrawDebugTrace::Type)AttackCheckDebugType, HitResult, true);

			bool bFindResult = UKismetSystemLibrary::SphereTraceSingleForObjects(GetOwner(), AttackCheckStart, AttackCheckEnd, 12,{ EObjectTypeQuery::ObjectTypeQuery3 }, false, AttackCheckIgnoreActor, (EDrawDebugTrace::Type)AttackCheckDebugType, HitResult, true);

			if (bFindResult)
			{
				AActor* TraceActor = HitResult.GetActor();
				FVector HitDir = (AttackCheckEnd - AttackCheckStart).GetSafeNormal();

				if(!AttackCheckIgnoreActor.Contains(TraceActor))
				{
					if (UAttackFeelComponent* AttackFeelComponent = TraceActor->GetComponentByClass<UAttackFeelComponent>())
					{
						AttackFeelComponent->AttackFeel(GetOwner(), HitResult, HitDir);
					}
				}

				if(GAttackCheckDebugType == 4)
				{
					FVector ArrowStart = HitResult.ImpactPoint;
					FVector ArrowEnd = ArrowStart + HitDir * 50;

					UKismetSystemLibrary::DrawDebugArrow(GetOwner(), ArrowStart, ArrowEnd, 16, FLinearColor::Green, 5);
					UKismetSystemLibrary::DrawDebugSphere(GetOwner(), ArrowStart, 16, 12, DebugHitColor, 5);
				}
				

				AttackCheckIgnoreActor.Add(TraceActor);
			}

			PreAttackCheckLocation[index] = AttackCheckEnd;
		}
	}
}

void UWeaponMeshComponent::StartAttackCheck()
{
	if(WeapongAttckSocketList.Num() == 0)
	{
		return;
	}

	AttackCheckIgnoreActor.Empty();

	PreAttackCheckLocation.Empty();
	for (auto WeapongAttckSocket : WeapongAttckSocketList)
	{
		PreAttackCheckLocation.Add(GetSocketLocation(WeapongAttckSocket));
	}

	bIsAttackChecking = true;
	SetComponentTickEnabled(true);
}

void UWeaponMeshComponent::EndAttackCheck()
{
	bIsAttackChecking = false;

	AttackCheckIgnoreActor.Empty();
	PreAttackCheckLocation.Empty();

	SetComponentTickEnabled(false);
}

void UWeaponMeshComponent::RestAttackCheck()
{
	AttackCheckIgnoreActor.Empty();
}

void UWeaponMeshComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(false);
	WeapongAttckSocketList = GetAllSocketNames();
}
