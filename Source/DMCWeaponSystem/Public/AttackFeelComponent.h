// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffect.h"
#include "AttackFeelComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DMCWEAPONSYSTEM_API UAttackFeelComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttackFeelComponent();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> PoiseDamageEffect;

	UPROPERTY(EditAnywhere)
	FGameplayTag DamageImmunityGameplayTag;

	UPROPERTY(EditAnywhere)
	FGameplayTag BlockingGameplayTag;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AttackFeel(AActor* AttackActor, const FHitResult& HitInfo, const FVector& HitDir);
};
