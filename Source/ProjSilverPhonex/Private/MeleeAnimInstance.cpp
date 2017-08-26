// Copyright Project Silver Phoneix

#include "MeleeAnimInstance.h"
#include "BasePlayer.h"
#include "BaseWeapon.h"
#include "MeleeWeapon.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"


void UMeleeAnimInstance::Attack(EAttackType AttackType)
{
	//Check if the player is attacking 
	//Set can jump to false, to stop the player from jumping
	bisAttacking = true;
	ABasePlayer* Player = Cast<ABasePlayer>(TryGetPawnOwner());
	ABaseWeapon* Weapon = Player->Inventory.CurrentWeapon;
	AMeleeWeapon* Melee = Cast<AMeleeWeapon>(Weapon);

	if (Player && Melee)
	{
		Player->SetCanJump(false);
		Player->Inventory.CurrentWeapon->StartAttack();

		if (AttackType == EAttackType::PS_Light)
		{

			//	Melee->GetLightAttackMontages();
			PlayCombo(EAttackType::PS_Light);
		}
		else if (AttackType == EAttackType::PS_Heavy)
		{
			//Melee->GetHeavyAttackMontages();
			PlayCombo(EAttackType::PS_Heavy);
		}

	}
}

void UMeleeAnimInstance::PlayCombo(EAttackType AttackType)
{
	ABasePlayer* Player = Cast<ABasePlayer>(TryGetPawnOwner());
	ABaseWeapon* Weapon = Player->Inventory.CurrentWeapon;
	AMeleeWeapon* Melee = Cast<AMeleeWeapon>(Weapon);

	//UE_LOG(LogTemp, Warning, TEXT("PlayCombo"));
	if (Player)
	{
		TArray<FWeaponAnimation> Montages;
		if (AttackType == EAttackType::PS_Light)
		{
			//
			//	UE_LOG(LogTemp, Warning, TEXT("Montage light"));
			Montages = Melee->GetLightAttackMontages();
		}
		else if (AttackType == EAttackType::PS_Heavy)
		{
			Montages = Melee->GetHeavyAttackMontages();
		}

		if (0 == Montages.Num()) { return; }

		//Can have a loop - instead mutiple ifs
		// ComboCounter == 0 - This bypass the base cass
		if (bAcceptNextCombo || ComboCounter == 0)
		{

			if (ComboCounter < Montages.Num())
			{
				//0 != Montages.Num() &&
				//UE_LOG(LogTemp, Warning, TEXT("Counter: %d"), ComboCounter);
				if (Montages[ComboCounter].LaunchCharacter)
				{
					//Launch Character
					Player->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
				}
				else
				{
					Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
				}

				//Clear array before each attack
				Melee->ClearEnemiesHitArray();//Clears array

										  // change direction after each attack montage
				ChangeDirection();
				//StartAttackingTrace();

				//Play appropriate animations 
				//Weapon->PlayWeaponAnimation(Montages[ComboCounter].MeleeAttackMontages);
				PlayAnimation(Montages[ComboCounter].MeleeAttackMontages);
				ComboCounter++;
				bAcceptNextCombo = false;//Stops the player from attacking again straight away
			}

		}
	}
}

void UMeleeAnimInstance::ComboReset()
{
	if (GetWorld())
	{

		float time = Montage_GetPosition(GetCurrentActiveMontage());
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &UMeleeAnimInstance::Reset, 0.6f, false);
		ABasePlayer* Player = Cast<ABasePlayer>(TryGetPawnOwner());
		if (Player)
		{
			Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
		//TODO - Maybe have different timers one to reset combo and other to return player back to passive
	}
}

void UMeleeAnimInstance::Reset()
{
	ABasePlayer* Player = Cast<ABasePlayer>(TryGetPawnOwner());
	ABaseWeapon* Weapon = Player->Inventory.CurrentWeapon;
	AMeleeWeapon* Melee = Cast<AMeleeWeapon>(Weapon);
	if (Player && Melee)
	{
		Melee->ClearEnemiesHitArray();//Clears array

		Player->SetCanJump(true);
		Player->SwitchStats(EPlayerStates::PS_Passive);
		Player->Inventory.CurrentWeapon->StopAttack();
	}

	ComboCounter = 0;
	bisAttacking = false;
}

void UMeleeAnimInstance::ChangeDirection()
{
	ABasePlayer* Player = Cast<ABasePlayer>(TryGetPawnOwner());
	//If player is locked on we dont allow the player to change direction
	if (Player && !Player->GetIsLockedOn())
	{


		MoveForwad = Player->GetMoveForward();
		MoveRight = Player->GetMoveRight();
		MoveRight *= -1;

		auto CameraLocation = Player->GetCamera()->GetComponentLocation();

		//This makes sure wheh for example presses up its relative to the camera 
		auto LookRotator = UKismetMathLibrary::FindLookAtRotation(CameraLocation, Player->GetActorLocation());
		auto Dire = UKismetMathLibrary::MakeVector(MoveForwad, MoveRight, 0.f);
		auto VectorLength = UKismetMathLibrary::VSize(Dire);
		//RotationFromXVector
		//UKismetMathLibrary::Conv_VectorToRotator(Dire);

		//Delta(Rotator)
		auto newRotationValue = UKismetMathLibrary::NormalizedDeltaRotator(LookRotator, UKismetMathLibrary::Conv_VectorToRotator(Dire)).Yaw;
		if (VectorLength > 0.01)
		{
			Player->SetActorRotation(UKismetMathLibrary::MakeRotator(0, 0, newRotationValue));
		}

		//Need to reset values after each call 
		MoveRight = 0.f;
		MoveForwad = 0.f;

	}
}


void UMeleeAnimInstance::StopAttackingTrace()
{
	ABasePlayer* Player = Cast<ABasePlayer>(TryGetPawnOwner());
	ABaseWeapon* Weapon = Player->Inventory.CurrentWeapon;
	AMeleeWeapon* Melee = Cast<AMeleeWeapon>(Weapon);

	if (Player && Weapon)
	{
		if (Melee)
		{
			Melee->StopTraceAttack();
		}
	}
}


void UMeleeAnimInstance::StartAttackingTrace()
{
	ABasePlayer* Player = Cast<ABasePlayer>(TryGetPawnOwner());
	ABaseWeapon* Weapon = Player->Inventory.CurrentWeapon;
	AMeleeWeapon* Melee = Cast<AMeleeWeapon>(Weapon);

	if (Player && Weapon)
	{
		if (Melee)
		{
			Melee->StartTraceAttack();
		}
	}

}

void UMeleeAnimInstance::SetAcceptNextCombo(bool NewState)
{
	bAcceptNextCombo = NewState;
}

float UMeleeAnimInstance::PlayAnimation(UAnimMontage * Animation, float InPlayRate, FName StartSectionName)
{
	float Duration = 0.0f;

	ABasePlayer* Player = Cast<ABasePlayer>(TryGetPawnOwner());
	if (Player)
	{
		if (Animation)
		{
			Duration = Player->PlayAnimMontage(Animation, 1.3, StartSectionName);
		}
	}

	return Duration;
}



