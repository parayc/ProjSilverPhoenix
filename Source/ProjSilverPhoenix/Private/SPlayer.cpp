// Copyright Project Silver Phoneix

#include "SPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnemyMaster.h"
#include "Kismet/KismetMathLibrary.h"
#include "CombatComponent.h"
#include "MeleeAnimInstance.h"
#include "ProjSilverPhoenix.h"
#include "BaseWeapon.h"
#include "MeleeWeapon.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "HealthComponent.h"



ASPlayer::ASPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BasePitchRate = 45.f;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.f;//Setting the length of the cameraboom
	SpringArm->SocketOffset = FVector(0, 50, 50);//Setting the position of the camera
	SpringArm->bUsePawnControlRotation = true;//Stops the player rotating with the camera

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);


}

void ASPlayer::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = Camera->FieldOfView;
	TargetFOV = DefaultFOV;

	DefaultSpringArmOffset = SpringArm->SocketOffset;

	//This restricts the players Camera movmevent from going to high or low 
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax = 70;
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMin = -70;

	HealthComponent = this->FindComponentByClass<UHealthComponent>();

	if (HealthComponent)
	{
		HealthComponent->OnHealthChange.AddDynamic(this, &ASPlayer::OnHealthChanged);
	}
	
}

void ASPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	IsEnemyInRange();
	IsEnemyOutOfRange();

	if (bIsLockedOn)
	{
		LockOnCamera(DeltaTime);
	}

	if (GetCharacterMovement()->IsMovingOnGround())
	{
		EndJump();
	}

	
	float NewFOV = FMath::FInterpTo(Camera->FieldOfView, TargetFOV, DeltaTime, 20.f);
	Camera->SetFieldOfView(NewFOV);


}

void ASPlayer::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("Turn", this, &ASPlayer::TurnRate);
	PlayerInputComponent->BindAxis("MoveForward", this, &ASPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASPlayer::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ASPlayer::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASPlayer::StartJump);
	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &ASPlayer::StartRoll);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ASPlayer::PressAttack);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &ASPlayer::ReleaseAttack);

	PlayerInputComponent->BindAction("Focus", IE_Pressed, this, &ASPlayer::PressFocus);
	PlayerInputComponent->BindAction("Focus", IE_Released, this, &ASPlayer::ReleaseFocus);


	PlayerInputComponent->BindAction("NextTarget", IE_Pressed, this, &ASPlayer::NextTarget);
	PlayerInputComponent->BindAction("PrevTarget", IE_Pressed, this, &ASPlayer::PrevTarget);

}

void ASPlayer::OnHealthChanged(UHealthComponent * OwningHealthComp, float Health, float HealthDelta, FVector HitDirection,  const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	//If We heal or do no damage just return
	if (HealthDelta <= 0) { return; }

	if (Health <= 0 && !GetIsDead())
	{
		//Death
		SetIsDead(true);
		OnDeath();

		return;
	}

	if (!ensure(CombatStates)) { return; }
	UMeleeAnimInstance* PlayerAnimation = Cast<UMeleeAnimInstance>(GetMesh()->GetAnimInstance());
	if (CombatStates->GetBattleState() == EBattleState::PS_Normal)
	{
		//Reset combo if we get hit
		if (PlayerAnimation)
		{
			PlayerAnimation->ResetComboAttack();
		}

		CombatStates->KnockBack(InstigatedBy->GetPawn(), this);
		CombatStates->Flinch(HitDirection);
	}
	else if (CombatStates->GetBattleState() == EBattleState::PS_SuperArmor && CombatStates->CalculateSuperArmor(HealthDelta))
	{
		//Reset combo if we get hit
		if (PlayerAnimation)
		{
			PlayerAnimation->ResetComboAttack();
		}
		CombatStates->KnockBack(InstigatedBy->GetPawn(), this);
		CombatStates->Flinch(HitDirection);
	}
}



void ASPlayer::MoveForward(float Value)
{
	//Needs to be called outside if statement to reset it back to zero
	MoveForwardAxisValue = Value;
	if ((Controller != NULL) && (Value != 0.0f) && !CombatStates->GetIsFlinching())
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		MoveForwardAxisValue = Value;
		AddMovementInput(Direction, Value);

	}
}

void ASPlayer::MoveRight(float Value)
{
	//Needs to be called outside if statement to reset it back to zero
	MoveRightAxisValue = Value;
	if ((Controller != NULL) && (Value != 0.0f) && !CombatStates->GetIsFlinching())
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		MoveRightAxisValue = Value;
		AddMovementInput(Direction, Value);

	}
}

void ASPlayer::TurnRate(float Rate)
{
	if (bIsLockedOn) return;
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASPlayer::LookUp(float Rate)
{
	if (bIsLockedOn) return;
	AddControllerPitchInput(Rate * BasePitchRate * GetWorld()->GetDeltaSeconds());
}

void ASPlayer::StartJump()
{

	if (CharacterEquipment.CurrentWeapon && CharacterEquipment.CurrentWeapon->GetIsAttcking())
	{
		 return; 
	}

	if (!CombatStates->GetIsFlinching() && GetIsRolling() != true)
	{
		SetIsJumping(true);
	}
}

void ASPlayer::EndJump()
{
	if (bIsJumping)
	{
		JumpCounter = 0;
		GetWorldTimerManager().ClearTimer(JumpResetHandle);
		bCanJump = true;
		bIsJumping = false;
	}
}

void ASPlayer::DoubleJump()
{
	//Launches the player 
	LaunchCharacter(FVector(0.f, 0.f, JumpHeight), false, true);
	//Resets the jump counter
	JumpCounter = 0;
}

void ASPlayer::SetIsJumping(bool NewState) 
{
	
	bIsJumping = NewState;
	if (bCanJump)
	{
		//Reset jump counter everytime before the first jump
		if (GetCharacterMovement()->IsMovingOnGround() == true)
		{
			JumpCounter = 0;
		}
		if (GetCharacterMovement()->IsMovingOnGround() == true && NewState)
		{

			Jump();//Makes the player jump
			JumpCounter++;
			//GetWorld()->GetTimerManager().SetTimer(JumpResetHandle, this, &ABasePlayer::EndJumping, 0.7f, false); //If Button is not pressed a second time before the timer, counter is reset

		}
		else if (GetCharacterMovement()->IsMovingOnGround() == false && NewState && JumpCounter >= 1)
		{
			DoubleJump();
			//We need to reset the double jump soon after its press
			GetWorld()->GetTimerManager().SetTimer(DJumpResetHandle, this, &ASPlayer::EndDoubleJump, 0.2f, false);
			bIsDoubleJump = true;

		}
	}
}

void ASPlayer::SetCanJump(bool NewState)
{
	bCanJump = NewState;
}

void ASPlayer::EndDoubleJump()
{
	bIsDoubleJump = false;
	GetWorldTimerManager().ClearTimer(DJumpResetHandle);
}

void ASPlayer::CreateNoise(float Loudness)
{
	MakeNoise(Loudness, this, GetActorLocation());
}

bool ASPlayer::GetIsJumping() const
{
	return bIsJumping;
}

bool ASPlayer::GetIsDoubleJumping() const
{
	return bIsDoubleJump;
}


void ASPlayer::RollCoolDown()
{
	RollCounter = 0;
	GetWorldTimerManager().ClearTimer(RollCoolDownHandle);
}

void ASPlayer::StartRoll()
{
	if (bCanRoll && RollCounter < MaxAmountPlayerCanRoll && !CombatStates->GetIsFlinching() && GetCharacterMovement()->IsMovingOnGround() == true)
	{
		bIsRolling = true;
		SetCanRoll(false);
		RollCounter++;
		RollDircetion();
	}
}

void ASPlayer::EndRoll()
{
	CombatStates->SetBattleState(EBattleState::PS_Normal);
	bIsRolling = false;
	SetCanRoll(true);
	GetWorldTimerManager().ClearTimer(RollResetHandle);
	GetWorld()->GetTimerManager().SetTimer(RollResetHandle, this, &ASPlayer::RollCoolDown, RollCooldownTimer, false);
}

void ASPlayer::RollDircetion()
{
	UMeleeAnimInstance* AnimInst = Cast<UMeleeAnimInstance>(GetMesh()->GetAnimInstance());
	if (!ensure(AnimInst)) { return; }

	//Check if roll animation is set 
	if (!ensure(ForwardRoll) || !ensure(BackwardRoll) || !ensure(RightRoll) || !ensure(LeftRoll)) { return; }


	if (CombatStates)
	{
		CombatStates->SetBattleState(EBattleState::PS_Invincible);
	}

	float Duration = 0.f;
	if (!GetIsLockedOn())
	{
		//Play montage 
		Duration = AnimInst->PlayAnimation(ForwardRoll);

	}
	else
	{
		//
		auto PlayersVelocity = GetVelocity().GetSafeNormal();
		auto PlayersForwardDirection = GetActorForwardVector().GetSafeNormal();

		//DotProduct
		//auto DotDirection = FVector::DotProduct(PlayersVelocity, PlayersForwardDirection);
		//auto CrossDirection = FVector::CrossProduct(PlayersForwardDirection, PlayersVelocity).Z;

		if (GetMoveRight() >= 0.5)
		{
			//Play Right roll
			Duration = AnimInst->PlayAnimation(RightRoll);
		}
		else if (GetMoveRight() <= -0.5)
		{
			//Play left roll
			Duration = AnimInst->PlayAnimation(LeftRoll);
		}
		else if (GetMoveForward() >= 0.5 || GetMoveForward() == 0)
		{//Check if we are moving forward or standing still 
		 //PlayMontage
			Duration = AnimInst->PlayAnimation(ForwardRoll);
		}
		else if (GetMoveForward() <= -0.5)
		{
			//Play backward montage
			Duration = AnimInst->PlayAnimation(BackwardRoll);
		}

	}

	/*This calls the method once the animation is played*/
	GetWorld()->GetTimerManager().SetTimer(RollResetHandle, this, &ASPlayer::EndRoll, Duration - 0.3f, false);

	//Stops the player from tracing if they roll
	AnimInst->ResetComboAttack();
}

bool ASPlayer::GetCanRoll() const
{
	return bCanRoll;
}

bool ASPlayer::GetIsRolling() const
{
	return bIsRolling;
}

void ASPlayer::SetCanRoll(bool NewState)
{
	bCanRoll = NewState;
}

float ASPlayer::GetMoveForward() const
{
	return MoveForwardAxisValue;
}

float ASPlayer::GetMoveRight() const
{
	return MoveRightAxisValue;
}


void ASPlayer::IsEnemyInRange()
{
	TArray<FHitResult> HitResults;
	FVector Start = GetActorLocation();
	FVector End = Start + FVector(0, 0, 150);//Line trace a sphere around the player

											 //Setting the collision details
	FCollisionShape CollionShape;
	CollionShape.ShapeType = ECollisionShape::Sphere;
	CollionShape.SetSphere(LockOnSphereRadius);

	FCollisionQueryParams CollParam;
	CollParam.AddIgnoredActor(this);//Ignore ourself

									//We only look for pawn objects
	ECollisionChannel ECC = ECollisionChannel::ECC_Pawn;

	if (GetWorld()->SweepMultiByObjectType(HitResults, Start, End, FQuat::FQuat(), ECC, CollionShape, CollParam))
	{
		for (FHitResult& Hits : HitResults)
		{
			AEnemyMaster* Enemy = Cast<AEnemyMaster>(Hits.GetActor());
			
			if (Enemy && !LockOnListTarget.Contains(Enemy) && IsTargetWithinSight(Enemy) && !Enemy->GetIsDead())
				LockOnListTarget.Add(Enemy);
			
		}
	}
}


bool ASPlayer::IsTargetWithinSight(AActor * Target)
{
	if (Target == nullptr) return false;

	FHitResult Hit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);

	FVector StartTrace = this->GetActorLocation();
	StartTrace.Z += 80.f; // Offset the line trace to start it from the players head 

	FVector EndTrace = Target->GetActorLocation();
	EndTrace.Z += 80.f;
	//Check Whether the enemy is within sight 
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_TargetSystem, TraceParams))
	{
		//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 0, 0), true, 10.f);
		AEnemyMaster *target = Cast<AEnemyMaster>(Hit.GetActor());
		if (target)
			return true;
		
	}
	
	return false;
}

void ASPlayer::LockOnCamera(float DeltaSec)
{
	if (bIsLockedOn)
	{
		//	UKismetSystemLibrary::FindLookAtRotation(GetActorLocation(), LockOnListTarget[0]);
		auto PlayerLocation = GetActorLocation();
		if (LockOnListTarget[TargetIndex])
		{
			auto Target = LockOnListTarget[TargetIndex]->GetActorLocation();

			//Same thing as UKismetMathLibrary::FindLookAtRotation(PlayerLocation,Target);
			auto Direction = (Target - PlayerLocation);
			auto LookDir = UKismetMathLibrary::MakeRotFromX(Direction);
			float Offset = -20.f;
			FRotator NewTarget(Offset + LookDir.Pitch, LookDir.Yaw, 0);
			GetController()->SetControlRotation(UKismetMathLibrary::RInterpTo(GetControlRotation(), NewTarget, DeltaSec, 20));
		}

	}
}

//Removes enemy from lock on list if they are out of sight or far from the player
void ASPlayer::IsEnemyOutOfRange()
{
	if (LockOnListTarget.Num() <= 0 || bIsLockedOn) return;

	for (int i = 0; i < LockOnListTarget.Num(); i++)
	{
		auto Target = LockOnListTarget[i];
		float Distance = (Target->GetActorLocation() - GetActorLocation()).Size();
		if (LockOnSphereRadius < Distance || !IsTargetWithinSight(Target))
		{
			LockOnListTarget.Remove(Target);
		}
	}

}



void ASPlayer::LockOn()
{
	if (LockOnListTarget.Num() <= 0) return;

	FHitResult HitResults;
	FVector Start = GetActorLocation();
	FVector End = Start + (GetActorForwardVector() * 500);

	//We only look for pawn objects
	ECollisionChannel ECC = ECollisionChannel::ECC_Pawn;

	FCollisionShape CollionShape;
	CollionShape.ShapeType = ECollisionShape::Box;
	CollionShape.Box.HalfExtentX = 20.f;
	CollionShape.Box.HalfExtentY = 20.f;
	CollionShape.Box.HalfExtentZ = 20.f;

	FCollisionQueryParams CollParam;
	CollParam.AddIgnoredActor(this);

	//I choose ECC_Weapon as it would always be blocking the enemy  
	if (GetWorld()->SweepSingleByChannel(HitResults,Start,End, FQuat::FQuat(),ECC_Weapon, CollionShape,CollParam))
	{
		auto Enemy = Cast<AEnemyMaster>(HitResults.GetActor());
		if (Enemy)
		{
			if (LockOnListTarget.Find(Enemy, TargetIndex))
			{
				LockOnListTarget[TargetIndex]->SetTargetIconHidden(false); 
			}
				
		}
		else
		{
			LockOnListTarget[0]->SetTargetIconHidden(false);
		}

	}
	else 
	{
		LockOnListTarget[0]->SetTargetIconHidden(false);
	}

	
	bIsLockedOn = true;

	//Make player face in the same direction as the camera
	LockPlayerToCameraView(bIsLockedOn);
	

}

void ASPlayer::LockOff()
{
	if (bIsLockedOn)
	{
		bIsLockedOn = false;
		//Release the player from look direction
		LockPlayerToCameraView(bIsLockedOn);

		//Check to see if the enemy is not dead
		if (LockOnListTarget[TargetIndex] != nullptr)
		{
			//the target icon 
			LockOnListTarget[TargetIndex]->SetTargetIconHidden(true);
		}

		//Empty the array
		LockOnListTarget.Empty();
		TargetIndex = 0;
	}
}

void ASPlayer::LockPlayerToCameraView(bool bLockPLayerView)
{
	bUseControllerRotationYaw = bLockPLayerView;
	GetCharacterMovement()->bUseControllerDesiredRotation = !bLockPLayerView;
	GetCharacterMovement()->bOrientRotationToMovement = !bLockPLayerView;
}

void ASPlayer::ZoomCamera(bool Zoom, FVector CameraOffset, float FieldOfViw)
{
	if (Zoom)
	{
		TargetFOV = FieldOfViw;
		SpringArm->SocketOffset += CameraOffset;
		return;
	}

	ResetCameraPosistion();
}

void ASPlayer::RemoveEnemyFromTargeting(AEnemyMaster * Target)
{
	if(Target)
	{
		LockOnListTarget.Remove(Target);
	}
}

void ASPlayer::ResetCameraPosistion()
{
	TargetFOV = DefaultFOV;
	SpringArm->SocketOffset = DefaultSpringArmOffset;
}

FVector ASPlayer::GetPawnViewLocation() const
{
	if (Camera)
	{
		return Camera->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
}

void ASPlayer::NextTarget()
{

	if (LockOnListTarget.Num() == 0 || LockOnListTarget.Num() == 1) return;

	//Check how much time has elasped since the last time the player tried to change  target
	float timeSincseLastPressed = GetWorld()->GetTimeSeconds() - LastTimePressed;
	if (0.5f < timeSincseLastPressed)
	{
		LastTimePressed = GetWorld()->GetTimeSeconds();
		if (bIsLockedOn)
		{

			//If we Reached the end of the list go back to the start

			//Remove the current target icon 
			if (LockOnListTarget[TargetIndex])
			{
				LockOnListTarget[TargetIndex]->SetTargetIconHidden(true);
			}

			//set the new icon 
			if (TargetIndex == LockOnListTarget.Num() - 1)
			{
				TargetIndex = 0;
				LockOnListTarget[TargetIndex]->SetTargetIconHidden(false);
			}
			else
			{
				TargetIndex++;
				LockOnListTarget[TargetIndex]->SetTargetIconHidden(false);
			}
		}
	}
	
	
}

void ASPlayer::PrevTarget()
{
	if (LockOnListTarget.Num() == 0 || LockOnListTarget.Num() == 1) return;

	//Check how much time has elasped since the last time the player tried to change  target
	float timeSincseLastPressed = GetWorld()->GetTimeSeconds() - LastTimePressed;
	
	if (0.5f < timeSincseLastPressed)
	{
		LastTimePressed = GetWorld()->GetTimeSeconds();
		if (bIsLockedOn)
		{

			//Remove the current target icon 
			LockOnListTarget[TargetIndex]->SetTargetIconHidden(true);

			//if at the start of the list go to the end of the list 
			if (LockOnListTarget[TargetIndex] == LockOnListTarget[0])
			{
				TargetIndex = LockOnListTarget.Num() - 1;
				LockOnListTarget[TargetIndex]->SetTargetIconHidden(false);
			}
			else
			{
				TargetIndex--;
				LockOnListTarget[TargetIndex]->SetTargetIconHidden(false);
			}
		}
	}
}

bool ASPlayer::GetIsLockedOn() const
{
	return bIsLockedOn;
}

void ASPlayer::PressAttack()
{
	//Stop the player from attacking if rolling
	if (GetIsRolling() == true || CombatStates->GetIsFlinching() == true) { return; }
	if (CharacterEquipment.CurrentWeapon)
	{
		CharacterEquipment.CurrentWeapon->StartAttack();
	}
}

void ASPlayer::ReleaseAttack()
{
	if (CharacterEquipment.CurrentWeapon)
		CharacterEquipment.CurrentWeapon->ReleaseAttack();
}

void ASPlayer::PressFocus()
{
	if (!CharacterEquipment.CurrentWeapon)
		LockOn();
	
	if (CharacterEquipment.CurrentWeapon)
	{
		CharacterEquipment.CurrentWeapon->PressFocus();
		AttachWeaponToSocket(CharacterEquipment.CurrentWeapon);
	}
}

void ASPlayer::ReleaseFocus()
{
	if (!CharacterEquipment.CurrentWeapon)
		LockOff();

	if (CharacterEquipment.CurrentWeapon)
		CharacterEquipment.CurrentWeapon->ReleaseFocus();
	
}

void ASPlayer::OnDeath()
{
	Super::OnDeath();
	OnDeathRequest.Broadcast();
	LockOff();
	auto playerController = Cast<APlayerController>(this->GetController());
	DisableInput(playerController);
	
}

void ASPlayer::SetCanAttack(bool NewState)
{
}

