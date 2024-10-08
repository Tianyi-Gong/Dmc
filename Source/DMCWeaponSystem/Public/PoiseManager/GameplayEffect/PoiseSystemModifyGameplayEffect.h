// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "PoiseSystemModifyGameplayEffect.generated.h"

UENUM(BlueprintType)
enum class EArmorType : uint8
{
	ArmorLv_None,
	ArmorLv_Min,
	ArmorLv_Small,
	ArmorLv_Middle,
	ArmorLv_Large,
	ArmorLv_Max
};

UENUM(BlueprintType)
enum class EActionImpactForce : uint8
{
	ImpactForceLV_None,
	ImpactForceLV_Min,
	ImpactForceLv_Small,
	ImpactForceLv_Middle,
	ImpactForceLv_Large,
	ImpactForce_Max,
	ImpactForce_UpperCut
};

USTRUCT()
struct FActionPoiseCoonfigDataTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UAnimMontage* ActionMontage;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float PoiseDamageModifyCoefficient = 0;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float PoiseDefenceModifyCoefficient = 0;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	EActionImpactForce ActionImpactForce = EActionImpactForce::ImpactForceLv_Small;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EArmorType ArmorTypeOffset = EArmorType::ArmorLv_None;
};

USTRUCT()
struct FActionPoiseConfigData
{
	FActionPoiseConfigData()
	{
	}

	FActionPoiseConfigData(const FActionPoiseCoonfigDataTableRow& Data)
	{
		PoiseDamageModifyCoefficient = Data.PoiseDamageModifyCoefficient;
		PoiseDefenceModifyCoefficient = Data.PoiseDefenceModifyCoefficient;
		ActionImpactForce = Data.ActionImpactForce;
		ArmorTypeOffset = Data.ArmorTypeOffset;
	}

	GENERATED_USTRUCT_BODY();
public:
	UPROPERTY()
	float PoiseDamageModifyCoefficient = 0;

	UPROPERTY()
	float PoiseDefenceModifyCoefficient = 0;

	UPROPERTY()
	EActionImpactForce ActionImpactForce = EActionImpactForce::ImpactForceLv_Small;

	UPROPERTY()
	EArmorType ArmorTypeOffset = EArmorType::ArmorLv_None;
};
/**
 * 
 */
UCLASS()
class DMCWEAPONSYSTEM_API UPoiseSystemModifyGameplayEffect : public UGameplayEffect
{
	UPoiseSystemModifyGameplayEffect();
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FString MontageName;

	UPROPERTY()
	bool bIsResetEffect;
};


