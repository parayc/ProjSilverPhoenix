// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "EnemyMaster.h"
#include "SKFootSoldier.generated.h"

/**
 * 
 */

class USphereComponent;
class USkeletalMeshComponent;

UCLASS()
class PROJSILVERPHONEX_API ASKFootSoldier : public AEnemyMaster
{
	GENERATED_BODY()
	
	
		//attach weapon to enemy AI

public:
	UPROPERTY(EditDefaultsOnly)
	FName WeaponSocketName;

	void SetIsAttacking(bool NewState);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnPlayerEnterPickupBox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFomSweep, const FHitResult& SweepResult);


private:
	ASKFootSoldier();

	UPROPERTY(EditDefaultsOnly, Category = "SetUp")
	USkeletalMeshComponent* Weapon;

	UPROPERTY(EditDefaultsOnly, Category = "SetUp")
		UCapsuleComponent* DamageBox;
	
	UPROPERTY(EditDefaultsOnly, Category = "SetUp")
		int32 WeaponDamage;

	bool bIsAttacking = false;
};
