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

	virtual void StartAttack() override;

	virtual void StopAttack() override;

	void StartFire();

	void StopFire();

	int32 GetCurrentAmmoInClip() const;

	int32 GetMaxAmmo() const;

	int32 GetCurrentAmmo() const;

	int32 GetMaxAmmoClip() const;

	void UseAmmo();

	FVector GetAdjustedAim();

private:

	/*Firerate timer handle*/
	FTimerHandle FireRateHandle;

	bool bIsFiring = false;

	bool bCanFire = true;

	bool bIsReloading = false;

	float FireRate;

	 

	int32 CurrentAmmo;

	int32 MaxAmmoInClip;

	int32 MaxAmmoInGun;

	int32 CurrentAmmoInClip;
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
		FName MuzzleName;
	
	
};
