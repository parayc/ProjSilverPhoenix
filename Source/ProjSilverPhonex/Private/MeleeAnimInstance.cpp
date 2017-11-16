// Copyright Project Silver Phoneix

#include "MeleeAnimInstance.h"
#include "BaseWeapon.h"
#include "MeleeWeapon.h"
#include "XBaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SPlayer.h"


void UMeleeAnimInstance::Attack(EAttackType AttackType)
{
	//Check if the player is attacking 
	//Set can jump to false, to stop the player from jumping
	bisAttacking = true;
	

	AXBaseCharacter* CharacterPawn = Cast<AXBaseCharacter>(TryGetPawnOwner());
	ABaseWeapon* Weapon2 = CharacterPawn->CharacterEquipment.CurrentWeapon;
	AMeleeWeapon* Melee2 = Cast<AMeleeWeapon>(Weapon2);

	if (CharacterPawn && Melee2)
	{
	//	CharacterPawn->SetCanJump(false);
		//TODO - Change behaviour
		//CharacterPawn->CharacterEquipment.CurrentWeapon->StartAttack();

		if (AttackType == EAttackType::PS_Light)
		{
			
			PlayCombo(EAttackType::PS_Light);
		}
		else if (AttackType == EAttackType::PS_Heavy)
		{
			
			PlayCombo(EAttackType::PS_Heavy);
		}

	}

}

void UMeleeAnimInstance::PlayCombo(EAttackType AttackType)
{
	
	AXBaseCharacter* CharacterPawn = Cast<AXBaseCharacter>(TryGetPawnOwner());
	ABaseWeapon* Weapon2 = CharacterPawn->CharacterEquipment.CurrentWeapon;
	AMeleeWeapon* Melee = Cast<AMeleeWeapon>(Weapon2);

	if (CharacterPawn)
	{
		TArray<FWeaponAnimation> Montages;
		if (AttackType == EAttackType::PS_Light)
		{
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
					CharacterPawn->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
				}
				else
				{
					CharacterPawn->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
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
		SetAcceptNextCombo(false);
		float time = Montage_GetPosition(GetCurrentActiveMontage());
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &UMeleeAnimInstance::Reset, 0.6f, false);
		
		AXBaseCharacter* CharacterPawn = Cast<AXBaseCharacter>(TryGetPawnOwner());
		if (CharacterPawn)
		{
			CharacterPawn->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
		//TODO - Maybe have different timers one to reset combo and other to return player back to passive
	}
}

void UMeleeAnimInstance::Reset()
{
	//AXBaseCharacter* CharacterPawn = Cast<AXBaseCharacter>(TryGetPawnOwner());
	
	ASPlayer* CharacterPawn = Cast<ASPlayer>(TryGetPawnOwner());
	
	if (CharacterPawn)
	{
		//Clears array
		StopAttack();
		//Player->SetCanJump(true);

		//If not rolling play unequip animation
		if (CharacterPawn->GetIsRolling() == false)
		{
			CharacterPawn->SwitchStats(EPlayerStates::PS_Passive);
		}

		CharacterPawn->CharacterEquipment.CurrentWeapon->StopAttack();
	}

	ComboCounter = 0;
	
}

void UMeleeAnimInstance::StopAttack()
{
	auto CharacterPawn = Cast<AXBaseCharacter>(TryGetPawnOwner());
	ABaseWeapon* Weapon = CharacterPawn->CharacterEquipment.CurrentWeapon;
	

	if (AMeleeWeapon* Melee = Cast<AMeleeWeapon>(Weapon))
	{
		Melee->ClearEnemiesHitArray();//Clears array
	}
	
	bisAttacking = false;
}

void UMeleeAnimInstance::ChangeDirection()
{
	

	auto* CharacterPawn = Cast<ASPlayer>(TryGetPawnOwner());
	//If player is locked on we dont allow the player to change direction
	if (CharacterPawn && !CharacterPawn->GetIsLockedOn())
	{


		MoveForwad = CharacterPawn->GetMoveForward();
		MoveRight = CharacterPawn->GetMoveRight();
		MoveRight *= -1;

		auto CameraLocation = CharacterPawn->GetCamera()->GetComponentLocation();

		//This makes sure wheh for example presses up its relative to the camera 
		auto LookRotator = UKismetMathLibrary::FindLookAtRotation(CameraLocation, CharacterPawn->GetActorLocation());
		auto Dire = UKismetMathLibrary::MakeVector(MoveForwad, MoveRight, 0.f);
		auto VectorLength = UKismetMathLibrary::VSize(Dire);
		//RotationFromXVector
		//UKismetMathLibrary::Conv_VectorToRotator(Dire);

		//Delta(Rotator)
		auto newRotationValue = UKismetMathLibrary::NormalizedDeltaRotator(LookRotator, UKismetMathLibrary::Conv_VectorToRotator(Dire)).Yaw;
		if (VectorLength > 0.01)
		{
			CharacterPawn->SetActorRotation(UKismetMathLibrary::MakeRotator(0, 0, newRotationValue));
		}

		//Need to reset values after each call 
		MoveRight = 0.f;
		MoveForwad = 0.f;

	}
}


void UMeleeAnimInstance::StopAttackingTrace()
{

	AXBaseCharacter* CharacterPawn = Cast<AXBaseCharacter>(TryGetPawnOwner());
	ABaseWeapon* Weapon = CharacterPawn->CharacterEquipment.CurrentWeapon;
	AMeleeWeapon* Melee = Cast<AMeleeWeapon>(Weapon);

	if (CharacterPawn && Weapon)
	{
		if (Melee)
		{
			Melee->StopTraceAttack();
		}
	}
}


void UMeleeAnimInstance::StartAttackingTrace()
{
	
	AXBaseCharacter* CharacterPawn = Cast<AXBaseCharacter>(TryGetPawnOwner());
	ABaseWeapon* Weapon = CharacterPawn->CharacterEquipment.CurrentWeapon;
	AMeleeWeapon* Melee = Cast<AMeleeWeapon>(Weapon);


	if (CharacterPawn && Weapon)
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

	AXBaseCharacter* CharacterPawn = Cast<AXBaseCharacter>(TryGetPawnOwner());
	if (CharacterPawn)
	{
		if (Animation)
		{
			Duration = CharacterPawn->PlayAnimMontage(Animation, 1.3, StartSectionName);
		}
	}

	return Duration;
}



//UE_LOG(LogTemp, Warning, TEXT("PlayCombo"));
//if (Player)
//{
//	TArray<FWeaponAnimation> Montages;
//	if (AttackType == EAttackType::PS_Light)
//	{
//		//
//		//	UE_LOG(LogTemp, Warning, TEXT("Montage light"));
//		Montages = Melee->GetLightAttackMontages();
//	}
//	else if (AttackType == EAttackType::PS_Heavy)
//	{
//		Montages = Melee->GetHeavyAttackMontages();
//	}

//	if (0 == Montages.Num()) { return; }

//	//Can have a loop - instead mutiple ifs
//	// ComboCounter == 0 - This bypass the base cass
//	if (bAcceptNextCombo || ComboCounter == 0)
//	{

//		if (ComboCounter < Montages.Num())
//		{
//			//0 != Montages.Num() &&
//			//UE_LOG(LogTemp, Warning, TEXT("Counter: %d"), ComboCounter);
//			if (Montages[ComboCounter].LaunchCharacter)
//			{
//				//Launch Character
//				Player->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
//			}
//			else
//			{
//				Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
//			}

//			//Clear array before each attack
//			Melee->ClearEnemiesHitArray();//Clears array

//									  // change direction after each attack montage
//			ChangeDirection();
//			//StartAttackingTrace();

//			//Play appropriate animations 
//			//Weapon->PlayWeaponAnimation(Montages[ComboCounter].MeleeAttackMontages);
//			PlayAnimation(Montages[ComboCounter].MeleeAttackMontages);
//			ComboCounter++;
//			bAcceptNextCombo = false;//Stops the player from attacking again straight away
//		}

//	}
//}