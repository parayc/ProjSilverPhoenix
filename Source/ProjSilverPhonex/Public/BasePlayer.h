// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BasePlayer.generated.h"


class ABaseWeapon;
class UCameraComponent;
class AEnemyMaster;
class UCombatComponent;

USTRUCT(BlueprintType)
struct FPlayerInventory
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plyaer")
		class ABaseWeapon* CurrentWeapon;

};

UENUM(BlueprintType)
enum class EPlayerStates : uint8
{
	PS_Combat UMETA(DisplayName = "Combat"),
	PS_Passive UMETA(DisplayName = "Passive"),
	PS_Unknown UMETA(DisplayName = "Unknown")
};

UCLASS()
class PROJSILVERPHONEX_API ABasePlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasePlayer();

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

	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool GetIsJumping() const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool GetIsDoubleJumping() const;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
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

	/*Player*/

	UFUNCTION(BlueprintCallable, Category = "SetUp")
		float GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable, Category = "SetUp")
		float GetMaxHealth() const;

	/** Applies damage to the character */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


	UFUNCTION(BlueprintCallable, Category = "Animations")
	EPlayerStates GetCurrentState() const;

	void SwitchStats(EPlayerStates NewState);

	UFUNCTION(BlueprintCallable, Category = "Animations")
	void AttachWeaponToSocket();

	/*Equipent & Attacking*/

	UPROPERTY(BlueprintReadWrite)
	FPlayerInventory Inventory;

	void AddWeaponToOnventory(class ABaseWeapon* NewWeapon);

	void LightAttack();

	void HeavyAttack();

	void SetCanAttack(bool NewState);

	 UCameraComponent* GetCamera();

	
protected:


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enum")
	EPlayerStates CurrentPlayerState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* Camera;

	//The weapon that the player starts with
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<class ABaseWeapon> StartingWeaponBlueprint;

	UPROPERTY(EditDefaultsOnly)
	FName WeaponSocketName = FName("Melee");

	UPROPERTY(EditDefaultsOnly)
	FName BackSocketName = FName("Back");

	UPROPERTY(VisibleAnywhere, Category = "Stats")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float MaxHealth = 100.f;

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
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Roll")
		UAnimMontage* ForwardRoll;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Roll")
		UAnimMontage* BackwardRoll;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Roll")
		UAnimMontage* RightRoll;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Roll")
		UAnimMontage* LeftRoll;


	bool bIsLockedOn = false;

	bool bCanAttack = true;

	int TargetIndex = 0;

	TArray<AEnemyMaster*> LockOnListTarget;

	void LockOnCamera(float DeltaSec);

	void CheckTargetsWithinSight(TArray<FHitResult> ActorsHit);

	void AddToLockOnTarget(AEnemyMaster* Target);

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup")
		float LockOnSphereRadius = 600.f;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup")
		float BackwardsWalkSpeed;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup")
		float WalkSpeed;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup")
		float StrafingSpeed;

	float closetTargetDistance;

	UPROPERTY(Category = "SetUp", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCombatComponent* CombatStates;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	
};
