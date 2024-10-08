// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "WeaponMeshComponent.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class DMCWEAPONSYSTEM_API UWeaponMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

	UWeaponMeshComponent();

public:
	UPROPERTY(VisibleAnywhere)
	TArray<FName> WeapongAttckSocketList;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	UFUNCTION(BlueprintCallable)
	void StartAttackCheck();

	UFUNCTION(BlueprintCallable)
	void EndAttackCheck();

	UFUNCTION(BlueprintCallable)
	void RestAttackCheck();
protected:
	virtual void BeginPlay() override;

	TArray<AActor*> AttackCheckIgnoreActor;
	TArray<FVector> PreAttackCheckLocation;
	bool bIsAttackChecking;

	FLinearColor DebugHitColor;

};
