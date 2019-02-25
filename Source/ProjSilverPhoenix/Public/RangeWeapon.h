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

	//virtual void StartAttack() override;

	//virtual void StopAttack() override;

	//virtual void ReleaseAttack() override;

	int32 GetCurrentAmmoInClip() const;

	int32 GetMaxAmmo() const;

	int32 GetCurrentAmmo() const;

	int32 GetMaxAmmoClip() const;

	void UseAmmo();

	FVector GetAdjustedAim();
protected:
	/*Firerate timer handle*/
	FTimerHandle FireRateHandle;

	bool bIsFiring;

	bool bCanFire;

	bool bIsReloading;

	float FireRate;

	int32 CurrentAmmo;

	int32 MaxAmmoInClip;

	int32 MaxAmmoInGun;

	int32 CurrentAmmoInClip;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FName MuzzleName;
	
	
};
