// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "XBaseCharacter.h"
#include "SPlayer.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathRequest);

class AEnemyMaster;
class UCameraComponent;
class AEnemyMaster;
class UCombatComponent;
class UHealthComponent;

UCLASS()
class PROJSILVERPHOENIX_API ASPlayer : public AXBaseCharacter
{
	GENERATED_BODY()

protected: 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		class UCameraComponent* Camera;

	UFUNCTION()
	void OnHealthChanged(UHealthComponent* OwningHealthComp, float Health, float HealthDelta, FVector HitDirection ,const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent;


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

	/*Jumping*/
	void StartJump();

	void EndJump();

	void DoubleJump();

	void SetIsJumping(bool NewState);

	void SetCanJump(bool NewState);

	void EndDoubleJump();

	UFUNCTION(BlueprintCallable, Category = "Setup")
		void CreateNoise(float Loudness);

	UFUNCTION(BlueprintCallable, Category = "Setup")
		bool GetIsJumping() const;

	UFUNCTION(BlueprintCallable, Category = "Setup")
		bool GetIsDoubleJumping() const;

	UPROPERTY(EditDefaultsOnly, Category = "Setup | Movement")
		float JumpHeight = 600.f;

	
	
	/*Rolling*/

	void RollCoolDown();

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

	void LockOn();

	void LockOff();

	void RemoveEnemyFromTargeting(AEnemyMaster* Target);

	/*Equipent & Attacking*/

	void Attack();

	void HeavyAttack();

	void SetCanAttack(bool NewState);

	UCameraComponent* GetCamera();

	virtual void OnDeath() override;

	UPROPERTY(BlueprintAssignable)
	FOnDeathRequest OnDeathRequest;

	float LastTimePressed = 0.0f;

	
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

	FTimerHandle RollCoolDownHandle;

	FTimerHandle UnequipHandle;

	int RollCounter = 0;
	/*This is the maximum amount the player can roll immediately after they performed a roll*/
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup | Movement")
	int MaxAmountPlayerCanRoll = 2;

	/*This is the cooldown length once the player has reached the max time they can roll*/
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup | Movement")
		float RollCooldownTimer = 5.f;

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

	void IsEnemyInRange();

	bool IsTargetWithinSight(AActor* Target);

	void LockOnCamera(float DeltaSec);

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup | LockOnSystem")
	float LockOnSphereRadius = 1000.f;

	void IsEnemyOutOfRange();

};
