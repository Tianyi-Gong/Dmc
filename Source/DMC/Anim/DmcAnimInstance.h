// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DmcAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EMoveDirection2Way : uint8
{
	None,
	F,
	B
};
/**
 * 
 */
 UENUM(BlueprintType)
enum class EMoveDirection4Way : uint8
{
	None,
	F,
	R,
	B,
	L
};

UENUM(BlueprintType)
enum class EMoveDirection8Way : uint8
{
	None,
	F,
	FR,
	R,
	BR,
	B,
	BL,
	L,
	FL
};


USTRUCT(BlueprintType)
struct FLocmotion4WayAnimSet
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimSequence* FAnim;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimSequence* RAnim;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimSequence* BAnim;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimSequence* LAnim;
};


USTRUCT(BlueprintType)
struct FLocmotion8WayAnimSet
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimSequence* FAnim;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimSequence* FR45Anim;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimSequence* FR90Anim;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimSequence* BR90Anim;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimSequence* BR45nim;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimSequence* BAnim;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimSequence* BL45Anim;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimSequence* BL90Anim;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimSequence* FL90Anim;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimSequence* FL45Anim;
};



UCLASS()
class DMC_API UDmcAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	EMoveDirection8Way SelectDirection8WayFromAngle(float Angle,float DeadZoe,EMoveDirection8Way CurrenrtDirection,bool CurrentIsMove);

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	EMoveDirection4Way SelectDirection4WayFromAngle(float Angle, float DeadZoe, EMoveDirection4Way CurrenrtDirection, bool CurrentIsMove);

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	EMoveDirection2Way SelectDirection2WayFromAngle(float Angle, float DeadZoe, EMoveDirection2Way CurrenrtDirection, bool CurrentIsMove);

	virtual void NativeBeginPlay() override;
};
