// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackFeelComponent.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "PoiseManager/GameplayEffect/PoiseSystemModifyGameplayEffect.h"
#include "PoiseManager/PoiseAttributeSet.h"
#include "PoiseManager/PoiseManagerComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"

static bool GForeceLocalHit = false;
static FAutoConsoleVariableRef CVarForeceLocalHit(
	TEXT("Weapon.ForeceLocalHit"),
	GForeceLocalHit,
	TEXT("")
);

static bool GForeceReadyHitAnim = false;
static FAutoConsoleVariableRef CVarForeceReadyHitAnim(
	TEXT("Weapon.ForeceReadyHitAnim"),
	GForeceReadyHitAnim,
	TEXT("")
);

// Sets default values for this component's properties
UAttackFeelComponent::UAttackFeelComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAttackFeelComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UAttackFeelComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAttackFeelComponent::AttackFeel(AActor* Attacker, const FHitResult& HitInfo,const FVector& HitDir)
{
	IAbilitySystemInterface* AsAttackerAbilitySysInterface = Cast<IAbilitySystemInterface>(Attacker);
	IAbilitySystemInterface* AsOwnerAbilitySysInterface = Cast<IAbilitySystemInterface>(GetOwner());
	if(AsAttackerAbilitySysInterface && AsOwnerAbilitySysInterface)
	{
		UAbilitySystemComponent* AttackAbilitySystemComponent = AsAttackerAbilitySysInterface->GetAbilitySystemComponent();
		UAbilitySystemComponent* OwnerAbilitySystemComponent = AsOwnerAbilitySysInterface->GetAbilitySystemComponent();

		if(AttackAbilitySystemComponent && OwnerAbilitySystemComponent)
		{
			const UPoiseAttributeSet* AttackerPoiseAttributeSetConst = Cast<UPoiseAttributeSet>(AttackAbilitySystemComponent->GetAttributeSet(UPoiseAttributeSet::StaticClass()));
			const UPoiseAttributeSet* DefencerPoiseAttributeSetConst = Cast<UPoiseAttributeSet>(OwnerAbilitySystemComponent->GetAttributeSet(UPoiseAttributeSet::StaticClass()));

			if(AttackerPoiseAttributeSetConst == nullptr || DefencerPoiseAttributeSetConst == nullptr)
				return;

			if(DamageImmunityGameplayTag.IsValid() && OwnerAbilitySystemComponent->HasMatchingGameplayTag(DamageImmunityGameplayTag))
			{
				//Todo: This Deal Damage Effect
				return;
			}

			UPoiseManagerComponent* PoiseManagerComponent = GetOwner()->GetComponentByClass<UPoiseManagerComponent>();
			if(PoiseManagerComponent == nullptr)
				return;

			if (BlockingGameplayTag.IsValid() && OwnerAbilitySystemComponent->HasMatchingGameplayTag(BlockingGameplayTag))
			{
				EActionImpactForce ActionImpactForceType = AttackerPoiseAttributeSetConst->GetActionImpactForceAsEnumValue<EActionImpactForce>();
				PoiseManagerComponent->ReadyBlockAttackAnim(Attacker,HitInfo, HitDir, ActionImpactForceType);
				return;
			}

			if (PoiseDamageEffect.Get() != nullptr)
			{
				AttackAbilitySystemComponent->BP_ApplyGameplayEffectToTarget(PoiseDamageEffect, OwnerAbilitySystemComponent,1, FGameplayEffectContextHandle());
			}

			//EActionImpactForce ActionImpactForce = AttackerPoiseAttributeSetConst->GetActionImpactForceAsEnumValue<EActionImpactForce>();

			float Poise = DefencerPoiseAttributeSetConst->GetPoise();
			int ActionImpactForce = AttackerPoiseAttributeSetConst->GetActionImpactForce();
			int ArmorType = FMath::Clamp(DefencerPoiseAttributeSetConst->GetArmorType(), 0, (int)EArmorType::ArmorLv_Max);

			bool IsFullArmor = (ArmorType == (int)EArmorType::ArmorLv_Max);
			bool bApplayHitReactAnim = (Poise <= 0 || (ActionImpactForce > ArmorType && !IsFullArmor));

			bApplayHitReactAnim = (bApplayHitReactAnim && !GForeceLocalHit) || GForeceReadyHitAnim;

			EActionImpactForce ActionImpactForceType = AttackerPoiseAttributeSetConst->GetActionImpactForceAsEnumValue<EActionImpactForce>();
			if(bApplayHitReactAnim)
			{
				//Ready Hit Anim
				PoiseManagerComponent->ReadyHitAnim(Attacker, HitInfo, HitDir, ActionImpactForceType);
			}
			else
			{
				//Use Physice anim instance Hit Anim
				PoiseManagerComponent->TryEnableLocalHitEffet(Attacker, HitInfo, HitDir, ActionImpactForceType);
			}
		}
	}
}
