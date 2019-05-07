// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "RangeWeapon.h"
#include "Bow.generated.h"

/**
 * 
 */
class AProjectile;
class ASPlayer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBowCharhingSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDrawBowEndSignature);

UCLASS()
class PROJSILVERPHOENIX_API ABow : public ARangeWeapon
{
	GENERATED_BODY()

private:

	void FireArrow(FVector arrowVelocity, float arrowDamage);

	void OnFireEnd();

	UFUNCTION(BlueprintCallable, Category = "Bow")
	void SpawnArrow();

	FVector CalculateArrowVelocity(FVector endPoint);

	//This is called in the player anim instance when the bow begins to charge
	UFUNCTION(BlueprintCallable, Category = "Bow")
	void BowCharging();

	//this is called in the player anim instance when the bow charging animation is complete
	UFUNCTION(BlueprintCallable, Category = "Bow")
	void BowFullyCharged();

	void CalculateProjectileSpeed();

	void CalculateProjectileDamage();

	FVector AimDirection();

	bool CanFire();

	void Zoom(bool bZooming);

public:
	ABow();
	
	void StartAttack() override;

	void ReleaseAttack() override;

	void PressFocus() override;

	void ReleaseFocus() override;
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool GetIsDrawingBow() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool CanAttachToBowString() const;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileToShoot;

	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBowCharhingSignature OnBowCharged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDrawBowEndSignature OnDrawBowEnd;

	AProjectile* currentProjectile = nullptr;

	/*The socket on the bow the arrow attach to when its ready to fire*/
	UPROPERTY(EditAnywhere, Category = "Bow")
	FName ArrowRestSocket = "ArrowRestSocket";

	UPROPERTY(EditAnywhere, Category = "Bow")
	float minProjectileSpeed = 1900.f;

	UPROPERTY(EditAnywhere, Category = "Bow")
	float maxProjectileSpeed = 5500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* FireBowMontage;

	bool bIsDrawingBow = false;

	FTimerHandle BowDrawingTimeHandle;

	FTimerHandle BowDamageTimeHandle;
	
	FTimerHandle OnFireEndTimeHandle;

	UPROPERTY(EditAnywhere, Category = "Bow")
	float BowDrawingChargeRate = 0.11;

	/*The starting launchSpeed value before bow ischarging*/
	UPROPERTY(EditAnywhere, Category = "Bow")
	float InitialLaunchSpeed = 37.f;

	/*Gets the percentage of the current launch speed and adds to it */
	UPROPERTY(EditAnywhere, Category = "Bow", meta =  (ClampMin = "0", ClampMax = "1"))
	float PercentageIncrease = 0.12f;

	UPROPERTY(EditAnywhere, Category = "Bow")
	float BaseDamage = 6.f;

	UPROPERTY(EditAnywhere, Category = "Bow")
	float MaxDamage = 45.f;

	float CurrentDamage;

	float LaunchSpeed;
	/*This offset the camera to the left or the right of the owning pawn*/
	UPROPERTY(EditAnywhere, Category = "Bow")
	FVector CameraOffset;

	UPROPERTY(EditAnywhere, Category = "Bow")
	float ZoomFOV = 55.f;
};
