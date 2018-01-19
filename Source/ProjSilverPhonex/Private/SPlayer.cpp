// Copyright Project Silver Phoneix

#include "SPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnemyMaster.h"
#include "Kismet/KismetMathLibrary.h"
#include "CombatComponent.h"
#include "MeleeAnimInstance.h"
#include "ProjSilverPhonex.h"
#include "BaseWeapon.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

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

	//This restricts the players Camera movmevent from going to high or low 
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax = 70;
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMin = -70;
	
}

void ASPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsLockedOn)
	{
		LockOnCamera(DeltaTime);
	}

	if (GetCharacterMovement()->IsMovingOnGround())
	{
		EndJump();
	}

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
	PlayerInputComponent->BindAction("LightAttack", IE_Pressed, this, &ASPlayer::Attack);
	PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, &ASPlayer::HeavyAttack);

	PlayerInputComponent->BindAction("LockOn", IE_Pressed, this, &ASPlayer::FindLockOnTargets);
	PlayerInputComponent->BindAction("LockOn", IE_Released, this, &ASPlayer::LockOff);

	PlayerInputComponent->BindAction("NextTarget", IE_Pressed, this, &ASPlayer::NextTarget);
	PlayerInputComponent->BindAction("PrevTarget", IE_Pressed, this, &ASPlayer::PrevTarget);

}



void ASPlayer::MoveForward(float Value)
{
	//Needs to be called outside if statement to reset it back to zero
	MoveForwardAxisValue = Value;
	if ((Controller != NULL) && (Value != 0.0f) && !IsFlinching() )
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
	if ((Controller != NULL) && (Value != 0.0f) && !IsFlinching())
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
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASPlayer::LookUp(float Rate)
{
	AddControllerPitchInput(Rate * BasePitchRate * GetWorld()->GetDeltaSeconds());
}


void ASPlayer::StartJump()
{
	if (!IsFlinching() && GetIsRolling() != true)
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
	if (bCanRoll && RollCounter < MaxAmountPlayerCanRoll && !IsFlinching() && GetCharacterMovement()->IsMovingOnGround() == true)
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
		//UE_LOG(LogTemp, Warning, TEXT("Roll"))
	}
	else
	{
		//
		auto PlayersVelocity = GetVelocity().GetSafeNormal();
		auto PlayersForwardDirection = GetActorForwardVector().GetSafeNormal();

		//DotProduct
		auto DotDirection = FVector::DotProduct(PlayersVelocity, PlayersForwardDirection);
		auto CrossDirection = FVector::CrossProduct(PlayersForwardDirection, PlayersVelocity).Z;


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
			auto Direction(Target - PlayerLocation);
			auto LookDir = UKismetMathLibrary::MakeRotFromX(Direction);
			FRotator NewTarget(-20, LookDir.Yaw, 0);


			GetController()->SetControlRotation(UKismetMathLibrary::RInterpTo(GetControlRotation(), NewTarget, DeltaSec, 20));
		}

	}
}

void ASPlayer::CheckTargetsWithinSight(TArray<FHitResult> ActorsHit)
{
	for (FHitResult Actors : ActorsHit)
	{

		//Type checked 
		AEnemyMaster *NewTarget = Cast<AEnemyMaster>(Actors.GetActor());
		if (NewTarget)
		{

			FHitResult Hit;
			FCollisionQueryParams TraceParams;
			TraceParams.AddIgnoredActor(this);
			
			FVector StartTrace = this->GetActorLocation();
			StartTrace.Z += 80.f; // Offset the line trace to start it from the players head 

			FVector EndTrace = NewTarget->GetActorLocation();
			EndTrace.Z += 80.f;
			//Check Whether the enemy is within sight 
			if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_TargetSystem, TraceParams))
			{

				//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 0, 0), true, 10.f);
				AEnemyMaster *target = Cast<AEnemyMaster>(Hit.GetActor());
				if (target && !LockOnListTarget.Contains(target))//Check to see if enemy is in the list already and type checking target
				{
					AddToLockOnTarget(target);
				}

			}

		}

	}

	//if target list is nor empty,  set the first target icon in the list visible 
	if (LockOnListTarget.Num() > 0)
	{
		LockOnListTarget[0]->SetTargetIconVisibility(false);
		//UE_LOG(LogTemp, Warning, TEXT("SetTargetIcon"))
	}
}

void ASPlayer::AddToLockOnTarget(AEnemyMaster * Target)
{
	//Stops the enemy from geting re-added just in case they are dead
	if (!Target->GetIsDead())
	{
		auto TargetDistance = GetDistanceTo(Target);
		//UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), TargetDistance)

		if (TargetDistance < closetTargetDistance)
		{
			//add it first to the list
			LockOnListTarget.Insert(Target, 0);
			closetTargetDistance = TargetDistance;
		}
		else
		{
			//Add to the end of the list
			LockOnListTarget.Add(Target);
		}

		//UE_LOG(LogTemp, Warning, TEXT("TargetLeangth: %d"), LockOnListTarget.Num())
		bIsLockedOn = true;

		//Make player face in the same direction as the camera
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
}



void ASPlayer::RemoveLockTarget(AEnemyMaster * TargetToRemove)
{
	LockOnListTarget.RemoveAt(TargetIndex);
	TargetIndex = 0;
}

void ASPlayer::FindLockOnTargets()
{
	TArray<FHitResult> HitResults;
	FVector Start = GetActorLocation();
	FVector End = Start + FVector(0, 0, 250);//Line trace a sphere around the player

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
		CheckTargetsWithinSight(HitResults);
	}
}

void ASPlayer::LockOff()
{
	if (bIsLockedOn)
	{
		//Reset closet target value
		closetTargetDistance = LockOnSphereRadius;
		//Release the player from look direction
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		bIsLockedOn = false;


		//Check to see if the enemy is dead
		if (LockOnListTarget[TargetIndex] != nullptr)
		{
			//Remove the target icon 
			LockOnListTarget[TargetIndex]->SetTargetIconVisibility(true);
		}

		//Empty the array
		LockOnListTarget.Empty();

		TargetIndex = 0;
	}
}

void ASPlayer::NextTarget()
{

	if (LockOnListTarget.Num() == 0 || LockOnListTarget.Num() == 1) return;

	if (bIsLockedOn)
	{
		//If we Reached the end of the list go back to the start

		//Remove the current target icon 
		if (LockOnListTarget[TargetIndex])
		{
			LockOnListTarget[TargetIndex]->SetTargetIconVisibility(true);
		}

		//set the new icon 
		if (TargetIndex == LockOnListTarget.Num() - 1)
		{
			TargetIndex = 0;
			LockOnListTarget[TargetIndex]->SetTargetIconVisibility(false);
		}
		else
		{
			TargetIndex++;
			LockOnListTarget[TargetIndex]->SetTargetIconVisibility(false);
		}
	}
}

void ASPlayer::PrevTarget()
{
	if (LockOnListTarget.Num() == 0 || LockOnListTarget.Num() == 1) return;

	if (bIsLockedOn)
	{

		//Remove the current target icon 
		LockOnListTarget[TargetIndex]->SetTargetIconVisibility(true);

		//if at the start of the list go to the end of the list 
		if (LockOnListTarget[TargetIndex] == LockOnListTarget[0])
		{
			TargetIndex = LockOnListTarget.Num() - 1;
			LockOnListTarget[TargetIndex]->SetTargetIconVisibility(false);
		}
		else
		{
			TargetIndex--;
			LockOnListTarget[TargetIndex]->SetTargetIconVisibility(false);
		}
	}
}

bool ASPlayer::GetIsLockedOn() const
{
	return bIsLockedOn;
}

void ASPlayer::Attack()
{
	//Stop the player from attacking if rolling
	if (GetIsRolling() == true ||  IsFlinching() == true) { return; }
	if (CharacterEquipment.CurrentWeapon)
	{
		CharacterEquipment.CurrentWeapon->StartAttack();
		if (EPlayerStates::PS_Passive == CurrentPlayerState)
		{
			//When we attack we go into combat state
			SwitchStats(EPlayerStates::PS_Combat);
			//When we attack, attach sword to hand
			AttachWeaponToSocket();


		}
	}
}


void ASPlayer::OnDeath()
{
	Super::OnDeath();
	OnDeathRequest.Broadcast();
	LockOff();
	auto playerController = Cast<APlayerController>(this->GetController());
	DisableInput(playerController);
	
}

void ASPlayer::HeavyAttack()
{
}

void ASPlayer::SetCanAttack(bool NewState)
{
}

UCameraComponent * ASPlayer::GetCamera()
{
	return Camera;
}


