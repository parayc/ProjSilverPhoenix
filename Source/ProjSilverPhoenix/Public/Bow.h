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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	void FireArrow(AProjectile* arrow, FVector arrowVelocity);

	void SpawnArrow(FVector endPoint);

	FVector AimDirection();
	
	bool CanFire();

	void Zoom(bool bZooming);

	AProjectile* currentProjectile = nullptr;
	UPROPERTY(EditAnywhere, Category = "Bow")
	float ZoomFOV = 70.f;

	UPROPERTY(EditAnywhere, Category = "Bow")
	FName ArrowSpawnSocket;

	UPROPERTY(EditAnywhere, Category = "Bow")
	float projectileSpeed = 5000.f;

	UPROPERTY(EditAnywhere, Category = "Bow")
	float maxprojectileSpeed = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* FireBowMontage;

	bool bIsDrawingBow = false;

	
};
