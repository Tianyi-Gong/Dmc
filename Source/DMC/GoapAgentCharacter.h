#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameAbilitySystem/GoapAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet/AIAttributeSetBase.h"
#include "GoapAgentCharacter.generated.h"

UCLASS()
class DMC_API AGoapAgentCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGoapAgentCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
/**/
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UAbilitySystemComponent* GetAbilitySystemComponent()const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	UGoapAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	UAIAttributeSetBase* AIAttributeSetBase;
};
