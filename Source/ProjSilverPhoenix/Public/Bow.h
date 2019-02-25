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

	 void StopAttack() override;

	 void ReleaseAttack() override;

	 void SpawnArrow();
	 //todo -c ahneg back
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileToShoot;
private:
	UPROPERTY(EditAnywhere, Category = "Bow")
		AProjectile* currentProjectile = nullptr;
	UPROPERTY(EditAnywhere, Category = "Bow")
	float LaunchSpeed = 5000.f;
	UPROPERTY(EditAnywhere, Category = "Bow")
	FName ArrowSocket;

	
};
