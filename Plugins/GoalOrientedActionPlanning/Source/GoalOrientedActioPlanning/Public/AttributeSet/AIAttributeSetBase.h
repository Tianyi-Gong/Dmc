// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AIAttributeSetBase.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class GOALORIENTEDACTIONPLANNING_API UAIAttributeSetBase : public UAttributeSet
{
	UAIAttributeSetBase();

	GENERATED_BODY()
public:
	//For HP
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAIAttributeSetBase,Health)

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAIAttributeSetBase, MaxHealth)

	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const override;
	
};
