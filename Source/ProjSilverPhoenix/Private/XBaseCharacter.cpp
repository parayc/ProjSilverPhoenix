// Copyright Project Silver Phoneix

#include "XBaseCharacter.h"
#include "BaseWeapon.h"
#include "CombatComponent.h"
#include "MeleeAnimInstance.h"
#include "SPlayer.h"
#include "InventoryComponent.h"
#include "SPlayerController.h"
#include "HealthComponent.h"
#include "Components/CapsuleComponent.h"
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

	if (StartingWeaponBlueprint)
	{

		//Spawn the weapon
		ABaseWeapon* Weapon = GetWorld()->SpawnActor<ABaseWeapon>(StartingWeaponBlueprint,
			GetMesh()->GetSocketLocation(WeaponSocketName),
			GetMesh()->GetSocketRotation(WeaponSocketName));

		AddWeaponToCharacterEquipment(Weapon);
	}

	HealthComponent = this->FindComponentByClass<UHealthComponent>();

	if (HealthComponent)
	{
		HealthComponent->OnHealthChange.AddDynamic(this, &AXBaseCharacter::OnHealthChanged);
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

void AXBaseCharacter::OnHealthChanged(UHealthComponent * OwningHealthComp, float Health, float HealthDelta, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	//If We heal or do no damage just return
	if (HealthDelta <= 0) { return; }

	if (Health <= 0 && !bIsDead)
	{
		UE_LOG(LogTemp, Log, TEXT("Health Change Death "))
		//Death
		bIsDead = true;
		OnDeath();
		//DetachFromControllerPendingDestroy();

		return; 

	}

	if (!ensure(CombatStates) ) { return ; }

	if (CombatStates->GetBattleState() == EBattleState::PS_Normal)
	{
		UE_LOG(LogTemp, Warning, TEXT("Combat Comp"))
		//Reset combo if we get hit
		UMeleeAnimInstance* PlayerAnimation = Cast<UMeleeAnimInstance>(GetMesh()->GetAnimInstance());
		if (PlayerAnimation)
		{
				PlayerAnimation->ResetComboAttack();
		}

		CombatStates->KnockBack(DamageCauser, this);
		CombatStates->Flinch();

	}

}

void AXBaseCharacter::OnDeath()
{

	//Play Death Montage
	AXBaseCharacter* OwnerPawn = Cast<AXBaseCharacter>(this);
	if (OwnerPawn)
	{
		if (DeathMontages.Num() > 0)
		{
			auto MontageToPlay = DeathMontages[FMath::RandRange(0, DeathMontages.Num() - 1 )];
			if (MontageToPlay)
			{
				OwnerPawn->PlayAnimMontage(MontageToPlay, 1.0f);
			}
			
		}
		 
	}

	UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (CharacterComp)
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CharacterComp->StopMovementImmediately();
		CharacterComp->DisableMovement();
		CharacterComp->SetComponentTickEnabled(false);
	}
	SetLifeSpan(10.f);
	if (CharacterEquipment.CurrentWeapon)
	{
		CharacterEquipment.CurrentWeapon->DestroyWeapon();
	}
	


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




bool AXBaseCharacter::CanUnequip() const
{
	auto Player = Cast<ASPlayer>(this);
	return  Player->GetIsRolling() != true && Player->GetIsJumping() != true && CombatStates->GetIsFlinching() != true && Player->GetIsDead() != true;
}

