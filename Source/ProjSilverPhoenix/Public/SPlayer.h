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

private:
	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnRate(float Rate);

	void LookUp(float Rate);

	/*Jumping*/
	void StartJump();

	void EndJump();

	void DoubleJump();

	void EndDoubleJump();

	/*Rolling*/

	void RollCoolDown();

	void StartRoll();

	void EndRoll();

	void RollDircetion();

	/*Camera & Target System*/

	void NextTarget();

	void PrevTarget();

	void ResetCameraPosistion();

	void ZoomToNewCameraPosition(float DeltaTime);

	/*Equipent & Attacking*/

	void PressAttack();

	void ReleaseAttack();

	void PressFocus();

	void ReleaseFocus();

	void IsEnemyInRange();

	bool IsTargetWithinSight(AActor* Target);

	void LockOnCamera(float DeltaSec);

	void IsEnemyOutOfRange();



protected: 

	UFUNCTION()
	void OnHealthChanged(UHealthComponent* OwningHealthComp, float Health, float HealthDelta, FVector HitDirection ,const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent;


public:

	ASPlayer();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* Camera;

	virtual void BeginPlay() override;

	void SetIsJumping(bool NewState);

	void SetCanJump(bool NewState);

	UFUNCTION(BlueprintCallable, Category = "Setup")
	bool GetIsJumping() const;

	UFUNCTION(BlueprintCallable, Category = "Setup")
	bool GetIsDoubleJumping() const;

	bool GetCanRoll() const;

	UFUNCTION(BlueprintCallable, Category = "Animations")
	bool GetIsRolling() const;

	void SetCanRoll(bool NewState);

	float GetMoveForward() const;

	float GetMoveRight() const;

	//We override it to return the camera location as the eye location
	virtual FVector GetPawnViewLocation() const;

	UFUNCTION(BlueprintCallable, Category = "SetUp")
	bool GetIsLockedOn() const;

	void LockOn();

	void LockOff();

	void RemoveEnemyFromTargeting(AEnemyMaster* Target);

	void LockPlayerToCameraView(bool bLockPLayerView);

	void ZoomCamera(bool Zoom, FVector CameraOffset, float FieldOfViw = 0.f);

	FVector GetDefaultSpringArmSocket();

	virtual void OnDeath() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		float BaseTurnRate;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BasePitchRate;

	UPROPERTY(EditDefaultsOnly, Category = "Setup | Movement", meta = (AllowPrivateAccess = "true"))
	float JumpHeight = 600.f;

	float DefaultFOV;

	float TargetFOV;

	FVector TargetOffset;

	float LastTimeTargetChanged = 0.0f;

	FVector DefaultSpringArmOffset;

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FOnDeathRequest OnDeathRequest;

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

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup | LockOnSystem")
	float LockOnSphereRadius = 1000.f;


};
