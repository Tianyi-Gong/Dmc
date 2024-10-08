// Fill out your copyright notice in the Description page of Project Settings.


#include "PoiseManager/PoiseAttributeSet.h"
#include "GameplayEffectExtension.h"

UPoiseAttributeSet::UPoiseAttributeSet():
    Poise(0),
    MaxPoise(100),
    PoiseRecover(33),
    PoiseDamage(25),
    PoiseDamageCoefficient(1),
    PoiseDefenceCoefficient(0),
    ArmorType(2)
{
}

bool UPoiseAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
    Super::PreGameplayEffectExecute(Data);
    return true;
}

void UPoiseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
}

void UPoiseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    if(Attribute == GetPoiseAttribute())
    {
    	NewValue = FMath::Clamp(NewValue, 0, GetMaxPoise());
    }
}

void UPoiseAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);
}
