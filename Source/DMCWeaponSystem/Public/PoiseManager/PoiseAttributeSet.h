// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "PoiseManager\GameplayEffect\PoiseSystemModifyGameplayEffect.h"
#include "PoiseAttributeSet.generated.h"

/**
 * 
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

#define ATTRIBUTE_ACCESSORS_ENUM(ClassName, PropertyName)\
template<typename T> \
T Get##PropertyName##AsEnumValue() const { \
	return (T)((int)Get##PropertyName()); \
}

UCLASS()
class DMCWEAPONSYSTEM_API UPoiseAttributeSet : public UAttributeSet
{
	UPoiseAttributeSet();
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	FGameplayAttributeData Poise;
	ATTRIBUTE_ACCESSORS(UPoiseAttributeSet, Poise)

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	FGameplayAttributeData MaxPoise;
	ATTRIBUTE_ACCESSORS(UPoiseAttributeSet, MaxPoise)

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	FGameplayAttributeData PoiseRecover;
	ATTRIBUTE_ACCESSORS(UPoiseAttributeSet, PoiseRecover)

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	FGameplayAttributeData PoiseDamage;
	ATTRIBUTE_ACCESSORS(UPoiseAttributeSet, PoiseDamage)

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	FGameplayAttributeData PoiseDamageCoefficient;
	ATTRIBUTE_ACCESSORS(UPoiseAttributeSet, PoiseDamageCoefficient)

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	FGameplayAttributeData PoiseDefenceCoefficient;
	ATTRIBUTE_ACCESSORS(UPoiseAttributeSet, PoiseDefenceCoefficient)

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	FGameplayAttributeData ActionImpactForce;
	ATTRIBUTE_ACCESSORS(UPoiseAttributeSet, ActionImpactForce)
	ATTRIBUTE_ACCESSORS_ENUM(UPoiseAttributeSet, ActionImpactForce)

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	FGameplayAttributeData ArmorType;
	ATTRIBUTE_ACCESSORS(UPoiseAttributeSet, ArmorType)
	ATTRIBUTE_ACCESSORS_ENUM(UPoiseAttributeSet, ArmorType)
		

	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
};
