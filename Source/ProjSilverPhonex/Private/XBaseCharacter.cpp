// Copyright Project Silver Phoneix

#include "XBaseCharacter.h"
#include "BaseWeapon.h"
#include "CombatComponent.h"
#include "MeleeAnimInstance.h"
#include "SPlayer.h"
#include "InventoryComponent.h"
#include "SPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"


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

	//Todo - set back to normal
	CurrentHealth = MaxHealth - 10.f;

	//CurrentPlayerState = EPlayerStates::PS_Passive;

	if (StartingWeaponBlueprint)
	{

		//Spawn the weapon
		ABaseWeapon* Weapon = GetWorld()->SpawnActor<ABaseWeapon>(StartingWeaponBlueprint,
			GetMesh()->GetSocketLocation(WeaponSocketName),
			GetMesh()->GetSocketRotation(WeaponSocketName));

		AddWeaponToCharacterEquipment(Weapon);
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


	if (GetWalkDirection() > 0.8f)//Check if walking forward
	{
		SetWalkSpeed(WalkSpeed);

	}
	else if (GetWalkDirection() < 0.8f && GetWalkDirection()  > -0.6f)//starfing
	{
		SetWalkSpeed(StrafingSpeed);
	}
	else
	{
		SetWalkSpeed(BackwardsWalkSpeed);
	}

}

void AXBaseCharacter::SetWalkSpeed(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

float AXBaseCharacter::GetWalkDirection()
{
	return FVector::DotProduct(GetVelocity().GetSafeNormal2D(), GetActorRotation().Vector());
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

	if (bIsDead == true || CombatStates->GetBattleState() == EBattleState::PS_Invincible)
	{
		return CurrentHealth;
	}

	CurrentHealth -= Damage;

	if (CurrentHealth <= 0)
	{
		OnDeath();
		return CurrentHealth;
	}

	
	if (!ensure(CombatStates)) { return CurrentHealth; }

	if (CombatStates->GetBattleState() == EBattleState::PS_Normal)
	{
		//Reset combo if we get hit
		UMeleeAnimInstance* PlayerAnimation = Cast<UMeleeAnimInstance>(GetMesh()->GetAnimInstance());
		if (PlayerAnimation)
		{
			PlayerAnimation->ResetComboAttack();
		}

		auto DamageInstigator = Cast<ACharacter>(DamageCauser);
		if (DamageInstigator)
		{

			CombatStates->KnockBack(DamageInstigator, this);
			CombatStates->Flinch();

		}
	}


	return CurrentHealth;
}

bool AXBaseCharacter::IsFlinching() const
{
	return CombatStates->GetIsFlinching();
}

void AXBaseCharacter::AddWeaponToInventory(ABaseWeapon* Weapon)
{
	auto controller = Cast<ASPlayerController>(GetController());
	if (controller)
	{
		auto Inventory = controller->FindComponentByClass<UInventoryComponent>();
		if (!ensure(Inventory)) { return; }

		if (Weapon)
		{
			Inventory->AddItem(Weapon->GetClass(), 1);
			Weapon->Destroy();
		}
	}

}


void AXBaseCharacter::AddWeaponToCharacterEquipment(ABaseWeapon * NewWeapon)
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

		//Add the existing weapon back to to the inventory
		if (CharacterEquipment.CurrentWeapon != nullptr)
		{
			AddWeaponToInventory(CharacterEquipment.CurrentWeapon);
		}

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

	//If we are dead dont switch states
	if (GetIsDead())
	{
		return;
	}
	CurrentPlayerState = NewState;

	if (CharacterEquipment.CurrentWeapon)
	{

		//If not rolling, flinching play unequip animation
		if (EPlayerStates::PS_Passive == CurrentPlayerState && CanUnequip())
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

bool AXBaseCharacter::GetIsDead() const
{
	return bIsDead;
}

void AXBaseCharacter::OnDeath()
{
	
	UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (CharacterComp)
	{
		CharacterComp->StopMovementImmediately();
		CharacterComp->DisableMovement();
		CharacterComp->SetComponentTickEnabled(false);
	}


	CharacterEquipment.CurrentWeapon->DestroyWeapon();
	
	bIsDead = true;

}

int32 AXBaseCharacter::GetTeamNumber()
{
	return TeamNumber;
}

bool AXBaseCharacter::CanUnequip() const
{
	auto Player = Cast<ASPlayer>(this);
	return  Player->GetIsRolling() != true && Player->GetIsJumping() != true && Player->IsFlinching() != true && Player->GetIsDead() != true;
}

