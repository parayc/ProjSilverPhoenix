// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "RangeWeapon.h"
#include "Bow.generated.h"

/**
 * 
 */
class AProjectile;

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
	
	 void AimBow();

	 void SpawnArrow();

	 UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool GetIsDrawingBow() const;

	 //todo -c ahneg back
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileToShoot;

private:
	
	AProjectile* currentProjectile = nullptr;

	UPROPERTY(EditAnywhere, Category = "Bow")
	float LaunchSpeed = 5000.f;

	UPROPERTY(EditAnywhere, Category = "Bow")
	FName ArrowSpawnSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* FireBowMontage;

	bool bIsDrawingBow = false;

	
};
