// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "XBaseCharacter.h"
#include "SPlayer.generated.h"

/**
 * 
 */
class AEnemyMaster;
class UCameraComponent;
class AEnemyMaster;
class UCombatComponent;

UCLASS()
class PROJSILVERPHONEX_API ASPlayer : public AXBaseCharacter
{
	GENERATED_BODY()

protected: 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		class UCameraComponent* Camera;
public:

	ASPlayer();

		/**Movement*/

		/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		float BaseTurnRate;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		float BasePitchRate;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnRate(float Rate);

	void LookUp(float Rate);

	float GetWalkDirection();

	void SetWalkSpeed(float Speed);

	/*Jumping*/
	void StartJump();

	void EndJump();

	void DoubleJump();

	void SetIsJumping(bool NewState);

	void SetCanJump(bool NewState);

	void EndDoubleJump();

	UFUNCTION(BlueprintCallable, Category = "Setup")
		bool GetIsJumping() const;

	UFUNCTION(BlueprintCallable, Category = "Setup")
		bool GetIsDoubleJumping() const;

	UPROPERTY(EditDefaultsOnly, Category = "Setup | Movement")
		float JumpHeight = 600.f;


	/*Rolling*/

	void StartRoll();

	void EndRoll();

	void RollDircetion();

	bool GetCanRoll() const;

	UFUNCTION(BlueprintCallable, Category = "Animations")
		bool GetIsRolling() const;

	void SetCanRoll(bool NewState);

	float GetMoveForward() const;

	float GetMoveRight() const;

	/*Target System*/

	void NextTarget();

	void PrevTarget();

	UFUNCTION(BlueprintCallable, Category = "SetUp")
		bool GetIsLockedOn() const;

	void RemoveLockTarget(AEnemyMaster* TargetToRemove);

	void FindLockOnTargets();

	void LockOff();


	/*Equipent & Attacking*/
	void LightAttack();

	void HeavyAttack();

	void SetCanAttack(bool NewState);

	UCameraComponent* GetCamera();


	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	bool bCanJump = true;

	bool bIsJumping = false;

	bool bIsDoubleJump = false;

	FTimerHandle RollResetHandle;

	FTimerHandle JumpResetHandle;

	FTimerHandle DJumpResetHandle;

	int JumpCounter = 0;

	bool bIsRolling = false;

	bool bCanRoll = true;

	float MoveForwardAxisValue = 0.f;

	float MoveRightAxisValue = 0.f;


	/*Roll Animations*/
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup | Movement")
		UAnimMontage* ForwardRoll;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup | Movement")
		UAnimMontage* BackwardRoll;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup | Movement")
		UAnimMontage* RightRoll;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup | Movement")
		UAnimMontage* LeftRoll;

	bool bIsLockedOn = false;

	bool bCanAttack = true;

	int TargetIndex = 0;

	TArray<AEnemyMaster*> LockOnListTarget;

	void LockOnCamera(float DeltaSec);

	void CheckTargetsWithinSight(TArray<FHitResult> ActorsHit);

	void AddToLockOnTarget(AEnemyMaster* Target);

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup | LockOnSystem")
		float LockOnSphereRadius = 600.f;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup | Movement")
		float BackwardsWalkSpeed;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup | Movement")
		float WalkSpeed;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup | Movement")
		float StrafingSpeed;

	float closetTargetDistance;

};
