// Copyright Project Silver Phoneix

#include "HealthComponent.h"
#include "GameFramework/Actor.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.

	CurrentHealth = 100.f;
	TeamNum = 255;
	
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::OnTakeDamage);
	}
}

void UHealthComponent::OnTakeDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	
	if (CurrentHealth <= 0)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	OnHealthChange.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
	
}

bool UHealthComponent::IsFriendly(AActor * ActorA, AActor * ActorB)
{
	//If an actor doesnt have no component we dont wont to do no more damage
	if (ActorA == nullptr || ActorB == nullptr)
	{
		return true;
	}

	auto* HealthCompA = Cast<UHealthComponent>(ActorA->GetComponentByClass(UHealthComponent::StaticClass()));
	auto* HealthCompB = Cast<UHealthComponent>(ActorB->GetComponentByClass(UHealthComponent::StaticClass()));

	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		return true;
	}

	if (HealthCompA->TeamNum == HealthCompB->TeamNum)
	{
		return true;
	}

	return false;
}

void UHealthComponent::Heal(float HealAmount)
{
	if (CurrentHealth <= 0.0f || HealAmount <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
	OnHealthChange.Broadcast(this, CurrentHealth, -HealAmount, nullptr, nullptr, nullptr);
}

float UHealthComponent::GetHealth() const
{
	return CurrentHealth;
}

float UHealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}


