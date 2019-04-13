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
	
	 void FireArrow(AProjectile* arrow, FVector arrowVelocity);

	 void SpawnArrow(FVector endPoint);

	 FVector AimDirection();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool GetIsDrawingBow() const;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileToShoot;

	//TODO - move this to base weapon
	ASPlayer* playerOwner;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
	AProjectile* currentProjectile = nullptr;

	UPROPERTY(EditAnywhere, Category = "Bow")
	FName ArrowSpawnSocket;

	UPROPERTY(EditAnywhere, Category = "Bow")
	float projectileSpeed = 5000.f;

	UPROPERTY(EditAnywhere, Category = "Bow")
	float MaxprojectileSpeed = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* FireBowMontage;

	bool bIsDrawingBow = false;

	
};
