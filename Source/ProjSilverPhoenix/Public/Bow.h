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

public:
	ABow();
	
	 void StartAttack() override;

	 void ReleaseAttack() override;

	 void PressFocus() override;

	 void ReleaseFocus() override;
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool GetIsDrawingBow() const;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileToShoot;
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnBowCharhingSignature OnBowCharged;
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnDrawBowEndSignature OnDrawBowEnd;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	void FireArrow(AProjectile* arrow, FVector arrowVelocity);

	void SpawnArrow(FVector endPoint);
	
	//This is called when the bow begins to charge
	UFUNCTION(BlueprintCallable, Category = "Bow")
	void BowCharging();
	//this is called when the bow charging animation is complete
	UFUNCTION(BlueprintCallable, Category = "Bow")
	void BowFullyCharged();

	void CalculateProjectileSpeed();

	FVector AimDirection();
	
	bool CanFire();

	void Zoom(bool bZooming);

	AProjectile* currentProjectile = nullptr;
	UPROPERTY(EditAnywhere, Category = "Bow")
	float ZoomFOV = 70.f;

	UPROPERTY(EditAnywhere, Category = "Bow")
	FName ArrowSpawnSocket = "Arrow";

	UPROPERTY(EditAnywhere, Category = "Bow")
	float minProjectileSpeed = 1900.f;

	UPROPERTY(EditAnywhere, Category = "Bow")
	float maxProjectileSpeed = 5500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* FireBowMontage;

	bool bIsDrawingBow = false;

	FTimerHandle BowDrawingTimeHandle;
	
	UPROPERTY(EditAnywhere, Category = "Bow")
	float BowDrawingChargeRate = 0.11;

	float LaunchSpeed = 0;
};
