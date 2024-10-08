// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoiseAttributeSet.h"
#include "GameplayEffect.h"
#include "PoiseManager/GameplayEffect/PoiseSystemModifyGameplayEffect.h"
#include "Engine/DataTable.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "PoiseManagerComponent.generated.h"

UENUM(BlueprintType)
enum class EHitReactDirXY8Way : uint8
{
	ReactDir_None,
	ReactDir_F,
	ReactDir_FR,
	ReactDir_R,
	ReactDir_BR,
	ReactDir_B,
	ReactDir_BL,
	ReactDir_L,
	ReactDir_FL
};

UENUM(BlueprintType)
enum class EHitReactDirXY4Way : uint8
{
	ReactDir_None,
	ReactDir_F,
	ReactDir_R,
	ReactDir_B,
	ReactDir_L,
};

USTRUCT(BlueprintType)
struct DMCWEAPONSYSTEM_API FSpecialHitAnimConfig
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UAnimMontage* SpecialHitAnimMontageStart;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UAnimMontage* SpecialHitAnimMontageLoop;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UAnimMontage* SpecialHitAnimMontageEnd;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DMCWEAPONSYSTEM_API UPoiseManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPoiseManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPoiseAttributeSet* PoiseAttributeSet;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> PoiseRecoverEffet;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> PoiseRecoverDisableEffect;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> PoiseRecoverDisableAfterFullEffect;

	UPROPERTY(EditAnywhere)
	FGameplayTag GoapHitGameplayTag;

	UPROPERTY(EditAnywhere)
	FGameplayTag GoapLowPoiseGameplayTag;

	UPROPERTY(EditAnywhere)
	FGameplayTag HitGameplayEventTag;

	UPROPERTY(EditAnywhere)
	FGameplayTag BlockAttackGameplayEventTag;

	UPROPERTY(EditAnywhere)
	FGameplayTag CheckAirFloatingStateGameplayTag;

	UPROPERTY(EditAnywhere)
	FGameplayTag AirFloatingEndGameplayTag;

	UPROPERTY()
	TMap<FString, FActionPoiseConfigData> ActionPoiseConfigDataMap;

	UFUNCTION()
	void OnMontageStart(UAnimMontage* Montage);

	UFUNCTION()
	void OnMontageBlendOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnOwnerMoveModeChange(ACharacter* Character, EMovementMode PreMoveMode, uint8 PreviousCustomMode);

	UFUNCTION(BlueprintCallable)
	void GetLastHitMontageAnim(bool& Result, bool& IsSpecialAnim,UAnimMontage*& OutHitMontage, FSpecialHitAnimConfig& OutSpecialHitAnimConfig);

	virtual void PoiseChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(EditAnywhere)
	UDataTable* ActionPoiseCoonfigDataTable;

	UPROPERTY(EditAnywhere)
	TMap<EHitReactDirXY4Way, UAnimMontage*> NormalHitAnimMontage;

	UPROPERTY(EditAnywhere)
	TMap<EHitReactDirXY4Way, UAnimMontage*> LargeHitAnimMontage;

	UPROPERTY(EditAnywhere)
	TMap<EActionImpactForce, FSpecialHitAnimConfig> SpecialHitAnimMontageSet;

	UPROPERTY()
	FTimeline LocalHitTime;

	UPROPERTY(EditAnywhere);
	UCurveFloat* LocalHitCurveFloat;

	UPROPERTY(EditAnywhere);
	float LocalHitCurveTimeLineLength;

	UPROPERTY()
	FTimeline BeatBackTimeLine;

	UPROPERTY(EditAnywhere);
	UCurveFloat* BeatBackCurveFloat;

	UPROPERTY(EditAnywhere);
	float BeatBackCurveTimeLineLength;

	UPROPERTY(BlueprintReadOnly)
	EHitReactDirXY4Way HitReactDirXY4Way;

	UPROPERTY(BlueprintReadOnly)
	EActionImpactForce HitActionImpactForce;

	UPROPERTY(BlueprintReadOnly)
	FVector HitWorldDir;

	void ReadyHitAnim(AActor* Attacker ,const FHitResult& HitInfo, const FVector& HitDir, EActionImpactForce ActionImpactForce);

	void TryDisEnableLocalHitEffet();
	bool TryEnableLocalHitEffet(AActor* Attacker, const FHitResult& HitInfo, const FVector& HitDir, EActionImpactForce ActionImpactForce);

	void ReadyBlockAttackAnim(AActor* Attacker, const FHitResult& HitInfo, const FVector& HitDir, EActionImpactForce ActionImpactForce);
private:
	UPROPERTY()
	UAbilitySystemComponent* OwnerAbilitySystemComponent;

	UPROPERTY()
	UPoiseSystemModifyGameplayEffect* PoiseSystemModifyGameplayEffect;

	UFUNCTION()
	void OnLocalHitCurveFloatUpdate(float NewValue);

	//void BeatBackStart();

	UFUNCTION()
	void OnLocalHitCurveFloatFinish();

	void BeatBackStart(bool bIsBlockAttack);

	UFUNCTION()
	void OnBeatBackCurveFloatUpdate(float NewValue);

	UFUNCTION()
	void OnBeatBackCurveFloatFinish();

	void UpdateHitReactDir(AActor* Attacker , const FHitResult& HitInfo, const FVector& HitDir, EActionImpactForce ActionImpactForce, bool bIsBlockAttack = false);

	USkeletalMeshComponent* LocalHitSkeletalMeshComponent;
	FName HitBoneName;

	FOnTimelineFloat OnLocalHitCurveFloatUpdateDelegate;
	FOnTimelineEvent OnLocalHitCurveFloatFinishDelegate;

	FOnTimelineFloat OnBeatBackCurveFloatUpdateDelegate;
	FOnTimelineEvent OnBeatBackCurveFloatFinishDelegate;

	FVector BeatBackStartLocation;
	FVector BeatBackDirection;
	float BeatBackScale;
};
