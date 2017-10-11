// Copyright Project Silver Phoneix

#include "XBaseCharacter.h"
#include "BaseWeapon.h"
#include "CombatComponent.h"


// Sets default values
AXBaseCharacter::AXBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AXBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth - 10.f;

	//CurrentPlayerState = EPlayerStates::PS_Passive;

	if (StartingWeaponBlueprint)
	{

		//Spawn the weapon
		ABaseWeapon* Weapon = GetWorld()->SpawnActor<ABaseWeapon>(StartingWeaponBlueprint,
			GetMesh()->GetSocketLocation(WeaponSocketName),
			GetMesh()->GetSocketRotation(WeaponSocketName));

		AddWeaponToOnventory(Weapon);
	}


	CombatStates = this->FindComponentByClass<UCombatComponent>();
	if (CombatStates)
	{
		CombatStates->SetBattleState(EBattleState::PS_Normal);
	}
}

// Called every frame
void AXBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AXBaseCharacter::GetCurrentHealth() const
{
	return CurrentHealth;
}

float AXBaseCharacter::GetMaxHealth() const
{
	return MaxHealth;
}

void AXBaseCharacter::SetAddCurrentHealth(float Value)
{
	if (CurrentHealth + Value < MaxHealth)
	{
		CurrentHealth += Value;
	}
	else
	{
		CurrentHealth = MaxHealth;
	}
	
}

void AXBaseCharacter::SetAddMaxHealth(float Value)
{
	MaxHealth = Value;
}





float AXBaseCharacter::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	CurrentHealth -= Damage;

	if (CurrentHealth <= 0)
	{
		IsDead = true;
	}

	
	if (!ensure(CombatStates)) { return CurrentHealth; }

	if (CombatStates->GetBattleState() == EBattleState::PS_Normal)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dmagae Causer: %s"), *DamageCauser->GetName());
		auto DamageInstigator = Cast<ACharacter>(DamageCauser);
		if (DamageInstigator)
		{

			CombatStates->KnockBack(DamageInstigator, this);
			CombatStates->Flinch();

		}
	}


	return CurrentHealth;
}

void AXBaseCharacter::AddWeaponToOnventory(ABaseWeapon * NewWeapon)
{

	if (NewWeapon)
	{
		if (CurrentPlayerState == EPlayerStates::PS_Combat)
		{
			NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);//Attching the new weapon 
		}
		else if (CurrentPlayerState == EPlayerStates::PS_Passive)
		{
			NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BackSocketName);
		}

		NewWeapon->SetOwningPawn(this);
		CharacterEquipment.CurrentWeapon = NewWeapon;
	}
}

void AXBaseCharacter::AttachWeaponToSocket()
{
	if (EPlayerStates::PS_Combat == CurrentPlayerState)
	{//SnapToTargetIncludingScale
		CharacterEquipment.CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
		//UE_LOG(LogTemp, Warning, TEXT("hand socket"));
	}
	else
	{
		CharacterEquipment.CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BackSocketName);
		
	}
}


void AXBaseCharacter::SetCanAttack(bool NewState)
{
	bCanAttack = NewState;
}

EPlayerStates AXBaseCharacter::GetCurrentState() const
{
	return CurrentPlayerState;
}

void AXBaseCharacter::SwitchStats(EPlayerStates NewState)
{
	CurrentPlayerState = NewState;
	if (CharacterEquipment.CurrentWeapon)
	{
		//If not rolling play unequip animation
		if (EPlayerStates::PS_Passive == CurrentPlayerState)
		{
			CharacterEquipment.CurrentWeapon->UnEquip();
		}
		else//If we are rolling just attach weapon to back
		{
			AttachWeaponToSocket();
		}
	}
	else
	{
		//No weapon we are in passive state
		CurrentPlayerState = EPlayerStates::PS_Passive;
	}
}
