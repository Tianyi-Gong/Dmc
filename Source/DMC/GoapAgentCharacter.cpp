#include "GoapAgentCharacter.h"

// Sets default values
AGoapAgentCharacter::AGoapAgentCharacter()
{
	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UGoapAbilitySystemComponent>(TEXT("AbilitySystem"));

	AIAttributeSetBase = CreateDefaultSubobject<UAIAttributeSetBase>(TEXT("AIAttributeSetBase"));

	AIAttributeSetBase->InitHealth(100);
	AIAttributeSetBase->InitMaxHealth(100);
	//AIAttributeSetBase->InitMaxPoise(100);
	//AIAttributeSetBase->InitPoise(0);
}

// Called when the game starts or when spawned
void AGoapAgentCharacter::BeginPlay()
{
	Super::BeginPlay();

	//AbilitySystemComponent->InitAbilityActorInfo(this, this);
	//AbilitySystemComponent->AddSpawnedAttribute(AIAttributeSetBase);
}

// Called every frame
void AGoapAgentCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AGoapAgentCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UAbilitySystemComponent* AGoapAgentCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
