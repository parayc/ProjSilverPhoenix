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

				PlayAnimation(Montages[ComboCounter].MeleeAttackMontages);
				//Depening on the animtion being played we set the damage of the sword
				Melee->SetDamage(Montages[ComboCounter].DamagePerAnimation);
				ComboCounter++;
				bAcceptNextCombo = false;//Stops the player from attacking again straight away

				//This sets  back to passive if the player hasnt attack in 10 seconds
				GetWorld()->GetTimerManager().SetTimer(ResetStanceHandle, this, &UMeleeAnimInstance::ResetStance, 5.0f, false);
			}

		}
	}


}

void UMeleeAnimInstance::ResetComboAttack()
{
	//Reset Character back to walking
	AXBaseCharacter* CharacterPawn = Cast<AXBaseCharacter>(TryGetPawnOwner());
	if (CharacterPawn)
	{
		CharacterPawn->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	
	//We reset the combo counter after a period of time to stop the player from attacking immediately after
	GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &UMeleeAnimInstance::Reset, 0.5f, false);
	SetAcceptNextCombo(false);

	//If we take take damage or so roll we need to stop the tracing on the sword as this will not get called if the attack animation doesnt complete
	StopAttackingTrace();
	
}

void UMeleeAnimInstance::Reset()
{

	ComboCounter = 0;
	GetWorld()->GetTimerManager().ClearTimer(ComboTimerHandle);
	
}

void UMeleeAnimInstance::ResetStance()
{

	GetWorld()->GetTimerManager().ClearTimer(ResetStanceHandle);

	ASPlayer* CharacterPawn = Cast<ASPlayer>(TryGetPawnOwner());

	if (CharacterPawn)
	{
		CharacterPawn->SwitchStats(EPlayerStates::PS_Passive);
	}
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

/*This is called in the blurptint notifiy when to stop tracing when it gets to a certain point in the attack animation */
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

/*This is called in the blurptint notifiy when to start tracing when it gets to a certain point in the attack animation */
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

