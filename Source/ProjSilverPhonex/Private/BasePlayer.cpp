// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "MeleeAnimInstance.h"
#include "ProjSilverPhonex.h"
#include "BaseWeapon.h"
#include "EnemyMaster.h"
#include "Kismet/KismetMathLibrary.h"
#include "CombatComponent.h"



// Sets default values
ABasePlayer::ABasePlayer()
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

	//GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	CombatStates = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat States"));

}

// Called when the game starts or when spawned
void ABasePlayer::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	CurrentPlayerState = EPlayerStatess::PS_Passive;


	//Closet target will never be bigger than the lock radius 
	closetTargetDistance = LockOnSphereRadius;

	//This restricts the players Camera movmevent from going to high or low 
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax = 70;
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMin = -70;

	if (StartingWeaponBlueprint)
	{

		//Spawn the weapon
		ABaseWeapon* Weapon = GetWorld()->SpawnActor<ABaseWeapon>(StartingWeaponBlueprint,
			GetMesh()->GetSocketLocation(WeaponSocketName),
			GetMesh()->GetSocketRotation(WeaponSocketName));

		AddWeaponToOnventory(Weapon);
	}

	if (CombatStates)
	{
		CombatStates->SetBattleState(EBattleState::PS_Normal);
	}
	
}

// Called every frame
void ABasePlayer::Tick(float DeltaTime)
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

// Called to bind functionality to input
void ABasePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Turn", this, &ABasePlayer::TurnRate);
	PlayerInputComponent->BindAxis("MoveForward", this, &ABasePlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABasePlayer::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ABasePlayer::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABasePlayer::StartJump);
	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &ABasePlayer::StartRoll);
	PlayerInputComponent->BindAction("LightAttack", IE_Pressed, this, &ABasePlayer::LightAttack);
	PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, &ABasePlayer::HeavyAttack);
	PlayerInputComponent->BindAction("LockOn", IE_Pressed, this, &ABasePlayer::FindLockOnTargets);
	PlayerInputComponent->BindAction("LockOn", IE_Released, this, &ABasePlayer::LockOff);

	PlayerInputComponent->BindAction("NextTarget", IE_Pressed, this, &ABasePlayer::NextTarget);
	PlayerInputComponent->BindAction("PrevTarget", IE_Pressed, this, &ABasePlayer::PrevTarget);
}


void ABasePlayer::MoveForward(float Value)
{
	//Needs to be called outside if statement to reset it back to zero
	MoveForwardAxisValue = Value;
	if ((Controller != NULL) && (Value != 0.0f))
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

void ABasePlayer::MoveRight(float Value)
{
	//Needs to be called outside if statement to reset it back to zero
	MoveRightAxisValue = Value;
	if ((Controller != NULL) && (Value != 0.0f))
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


float ABasePlayer::GetWalkDirection()
{
	float WalkDirection = FVector::DotProduct(GetVelocity().GetSafeNormal2D(), GetActorRotation().Vector());
	return WalkDirection;
}

void ABasePlayer::SetWalkSpeed(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void ABasePlayer::TurnRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABasePlayer::LookUp(float Rate)
{
	AddControllerPitchInput(Rate * BasePitchRate * GetWorld()->GetDeltaSeconds());
}

void ABasePlayer::StartJump()
{
	SetIsJumping(true);
}

void ABasePlayer::EndJump()
{
	if (bIsJumping)
	{

		JumpCounter = 0;
		GetWorldTimerManager().ClearTimer(JumpResetHandle);
		bCanJump = true;
		bIsJumping = false;
	}
}

void ABasePlayer::DoubleJump()
{
	//Launches the player 
	LaunchCharacter(FVector(0.f, 0.f, JumpHeight), false, true);
	//Resets the jump counter
	JumpCounter = 0;
}

void ABasePlayer::EndDoubleJump()
{
	bIsDoubleJump = false;
	GetWorldTimerManager().ClearTimer(DJumpResetHandle);
}
void ABasePlayer::SetIsJumping(bool NewState)
{
	//Reset jump counter everytime before the first jump
	bIsJumping = NewState;
	if (bCanJump)
	{
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
			GetWorld()->GetTimerManager().SetTimer(DJumpResetHandle, this, &ABasePlayer::EndDoubleJump, 0.2f, false);
			bIsDoubleJump = true;

		}
	}
}

void ABasePlayer::SetCanJump(bool NewState)
{
	bCanJump = NewState;
}


bool ABasePlayer::GetIsJumping() const
{
	return bIsJumping;
}

bool ABasePlayer::GetIsDoubleJumping() const
{
	return bIsDoubleJump;
}


void ABasePlayer::StartRoll()
{
	if (bCanRoll)
	{
		bIsRolling = true;
		SetCanRoll(false);
		RollDircetion();

	}
}

void ABasePlayer::EndRoll()
{
	bIsRolling = false;
	bCanRoll = true;
	GetWorldTimerManager().ClearTimer(RollResetHandle);
}

void ABasePlayer::RollDircetion()
{
	UMeleeAnimInstance* AnimInst = Cast<UMeleeAnimInstance>(GetMesh()->GetAnimInstance());
	if (!ensure(AnimInst)) { return; }

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

		//TODO - if is rolling stop player from unequip with bool

	}

	//switchstate back to passive 
	//SwitchStats(EPlayerStatess::PS_Passive);
	//reset combo and stop weapon trace

	//TODO - Stop player from attacking when rolling
	GetWorld()->GetTimerManager().SetTimer(RollResetHandle, this, &ABasePlayer::EndRoll, Duration - 0.4f, false);
	AnimInst->StopAttackingTrace();
	AnimInst->Reset();

}

bool ABasePlayer::GetCanRoll() const
{
	return bCanRoll;
}

bool ABasePlayer::GetIsRolling() const
{
	return bIsRolling;
}

void ABasePlayer::SetCanRoll(bool NewState)
{
	bCanRoll = NewState;
}

float ABasePlayer::GetMoveForward() const
{
	return MoveForwardAxisValue;
}


float ABasePlayer::GetMoveRight() const
{
	return MoveRightAxisValue;
}


void ABasePlayer::NextTarget()
{
	if (LockOnListTarget.Num() == 0 || LockOnListTarget.Num() == 1) return;

	if (bIsLockedOn)
	{
		//If we Reached the end of the list go back to the start

		//Remove the current target icon 
		if (LockOnListTarget[TargetIndex])
		{
			LockOnListTarget[TargetIndex]->SetTargetHidden(true);
		}

		//set the new icon 
		if (TargetIndex == LockOnListTarget.Num() - 1)
		{
			TargetIndex = 0;
			LockOnListTarget[TargetIndex]->SetTargetHidden(false);
		}
		else
		{
			TargetIndex++;
			LockOnListTarget[TargetIndex]->SetTargetHidden(false);
		}
	}
}

void ABasePlayer::PrevTarget()
{
	if (LockOnListTarget.Num() == 0 || LockOnListTarget.Num() == 1) return;

	if (bIsLockedOn)
	{

		//Remove the current target icon 
		LockOnListTarget[TargetIndex]->SetTargetHidden(true);

		//if at the start of the list go to the end of the list 
		if (LockOnListTarget[TargetIndex] == LockOnListTarget[0])
		{
			TargetIndex = LockOnListTarget.Num() - 1;
			LockOnListTarget[TargetIndex]->SetTargetHidden(false);
		}
		else
		{
			TargetIndex--;
			LockOnListTarget[TargetIndex]->SetTargetHidden(false);
		}
	}
}

void ABasePlayer::RemoveLockTarget(AEnemyMaster* TargetToRemove)
{
	LockOnListTarget.RemoveAt(TargetIndex);
	TargetIndex = 0;
}

void ABasePlayer::LockOnCamera(float DeltaSec)
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

void ABasePlayer::LockOff()
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
			LockOnListTarget[TargetIndex]->SetTargetHidden(true);
		}

		//Empty the array
		LockOnListTarget.Empty();

		TargetIndex = 0;
	}
}

void ABasePlayer::CheckTargetsWithinSight(TArray<FHitResult> ActorsHit)
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
			//Check Whether the enemy is within sight 
			if (GetWorld()->LineTraceSingleByChannel(Hit, this->GetActorLocation(), NewTarget->GetActorLocation(), ECC_TargetSystem, TraceParams))
			{
				
				//DrawDebugLine(GetWorld(), this->GetActorLocation(), NewTarget->GetActorLocation(), FColor(255, 0, 0), true, 10.f);
				AEnemyMaster *target = Cast<AEnemyMaster>(Hit.GetActor());
				if (target && !LockOnListTarget.Contains(target))//Check to see if enemy is in the list already and type checking target
				{
					AddToLockOnTarget(target);
				}

			}


		}

	}

	//Checks if target list is empty, if not set thr first target icon visible 
	if (LockOnListTarget.Num() > 0)
	{
		LockOnListTarget[0]->SetTargetHidden(false);
		//UE_LOG(LogTemp, Warning, TEXT("SetTargetIcon"))
	}
}

void ABasePlayer::AddToLockOnTarget(AEnemyMaster* Target)
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

void ABasePlayer::FindLockOnTargets()
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


bool ABasePlayer::GetIsLockedOn() const
{
	return bIsLockedOn;
}

float ABasePlayer::GetCurrentHealth() const
{
	return CurrentHealth;
}

float ABasePlayer::GetMaxHealth() const
{
	return MaxHealth;
}

float ABasePlayer::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	CurrentHealth -= Damage;
	if (CurrentHealth <= 0)
	{

	}


	if (CombatStates->GetBattleState() == EBattleState::PS_Normal)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Dmagae Causer: %s"), *DamageCauser->GetName());
		auto DamageCas = Cast<ACharacter>(DamageCauser);
		if (DamageCas)
		{

			CombatStates->KnockBack(DamageCas, this);
			CombatStates->Flinch();

		}
		//CombatStates->KnockBack(DamageCas, this);
		//Play flinch animation 

	}

	return CurrentHealth;
}

EPlayerStatess ABasePlayer::GetCurrentState() const
{
	return CurrentPlayerState;
}

void ABasePlayer::SwitchStats(EPlayerStatess NewState)
{
	CurrentPlayerState = NewState;
	if (Inventory.CurrentWeapon)
	{

		if (EPlayerStatess::PS_Passive == CurrentPlayerState && bIsRolling == false)
		{
			Inventory.CurrentWeapon->UnEquip();
		}
		else//If we are rolling just attach weapon to back
		{
			AttachWeaponToSocket();
		}
	}
	else 
	{
		//No weapon we are in passive state
		CurrentPlayerState = EPlayerStatess::PS_Passive;
	}
}

void ABasePlayer::AttachWeaponToSocket()
{
	if (EPlayerStatess::PS_Combat == CurrentPlayerState)
	{//SnapToTargetIncludingScale
		Inventory.CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
	}
	else
	{
		Inventory.CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BackSocketName);
	}
}

void ABasePlayer::AddWeaponToOnventory(class ABaseWeapon* NewWeapon)
{
	//NewWeapon->ChangeOwner(this);//Select to new gun
	if (Inventory.CurrentWeapon == nullptr)
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BackSocketName);

	}
	else
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);//Attching the new weapon to the weapon socket - New to update
	}

//	NewWeapon->SetOwningPawn(this);
	Inventory.CurrentWeapon = NewWeapon;
}

void ABasePlayer::LightAttack()
{
	if (Inventory.CurrentWeapon)
	{
		if (GetIsRolling() == true) { return; }//If Player is rolling they cant attack 

		if (EPlayerStatess::PS_Passive == CurrentPlayerState)
		{
			//When we attack we go into combat state
			SwitchStats(EPlayerStatess::PS_Combat);
			//When we attack, attach sword to hand
			AttachWeaponToSocket();
		}

		UMeleeAnimInstance* animation = Cast<UMeleeAnimInstance>(GetMesh()->GetAnimInstance());
		//Inventory.CurrentWeapon->StartAttack();
		animation->Attack(EAttackType::PS_Light);
	}
}

void ABasePlayer::HeavyAttack()
{
	if (Inventory.CurrentWeapon)
	{
		if (GetIsRolling() == true) { return; }//If Player is rolling they cant attack 

		if (EPlayerStatess::PS_Passive == CurrentPlayerState)
		{
			//When we attack we go into combat state
			SwitchStats(EPlayerStatess::PS_Combat);
			//When we attack, attach sword to hand
			AttachWeaponToSocket();
		}

		UMeleeAnimInstance* animation = Cast<UMeleeAnimInstance>(GetMesh()->GetAnimInstance());
		//Inventory.CurrentWeapon->StartAttack();
		animation->Attack(EAttackType::PS_Heavy);
	}
}

void ABasePlayer::SetCanAttack(bool NewState)
{
	bCanAttack = NewState;
}

UCameraComponent * ABasePlayer::GetCamera()
{
	return Camera;
}

