// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "RangeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PROJSILVERPHOENIX_API ARangeWeapon : public ABaseWeapon
{
	GENERATED_BODY()


public:

	ARangeWeapon();

	int32 GetCurrentAmmoInClip() const;

	int32 GetMaxAmmo() const;

	int32 GetCurrentAmmo() const;

	int32 GetMaxAmmoClip() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool GetIsFiring() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool SetIsFiring() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool GetIsAiming() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetIsAiming(bool isAiming);

	void UseAmmo();

	FVector GetAdjustedAim();

	void AimTowardsCrossHair();
protected:
	/*Firerate timer handle*/
	FTimerHandle FireRateHandle;

	bool bIsFiring = false;

	bool bCanFire;

	bool bIsReloading;

	float FireRate;

	int32 CurrentAmmo;

	int32 MaxAmmoInClip;

	int32 MaxAmmoInGun;

	int32 CurrentAmmoInClip;

	bool bIsAiming = false;
	
};
