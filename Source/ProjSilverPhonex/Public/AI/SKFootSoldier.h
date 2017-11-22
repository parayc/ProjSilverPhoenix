// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "EnemyMaster.h"
#include "SKFootSoldier.generated.h"

/**
 * 
 */

class USphereComponent;
class USphereComponent;
class USkeletalMeshComponent;

UCLASS()
class PROJSILVERPHONEX_API ASKFootSoldier : public AEnemyMaster
{
	GENERATED_BODY()
	
	
		//attach weapon to enemy AI

public:
	
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void SetIsAttacking(bool NewState);

	// Called every frame
	virtual void Tick(float DeltaTime) override;


	TArray<AActor*> EnemiesHitList;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnEnemyEnterDamageBox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFomSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnEnemyLeavesDamageBox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Attack")
		void ResetEnemyAttackList();


private:
	ASKFootSoldier();

	UPROPERTY(EditDefaultsOnly, Category = "SetUp")
	USphereComponent* AttackSphere;

	bool bIsAttacking = false;

	bool bEnemyInRange = false;
};
