// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "MeleeWeapon.generated.h"

/**
 * 
 */

class UAnimMontage;
class UParticleSystemComponent;
class AEnemyMaster;

USTRUCT(BlueprintType)
struct FWeaponAnimation
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetUp")
	class UAnimMontage* MeleeAttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetUp")
		int32 DamagePerAnimation = 0;
	//Some Montages 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetUp")
		bool LaunchCharacter;

};
UCLASS()
class PROJSILVERPHONEX_API AMeleeWeapon : public ABaseWeapon
{
	GENERATED_BODY()


	AMeleeWeapon();

public:

	virtual void Tick(float DeltaTime) override;

	virtual void StartAttack() override;

	virtual void StopAttack() override;

	void AttackTrace();

	void SetDamage(int32 Value);

	void DealDamage(const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, Category = "Setup")
	bool GetIsAttcking();

	//Starts weapons line trace 
	void StartTraceAttack();

	//Ends weapons line trace 
	void StopTraceAttack();

	//Clears array
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void ClearEnemiesHitArray();

	TArray<FWeaponAnimation> GetLightAttackMontages();

	TArray<FWeaponAnimation> GetHeavyAttackMontages();

	TArray<FWeaponAnimation> GetAirAttackMontages();

	void SetSwordTrailVisibility(bool NewState);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FX")
	UParticleSystemComponent* SwordTrail;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:

	TArray<AXBaseCharacter*> EnemiesHit;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
		FName SocketBase;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
		FName SocketTip;

	UPROPERTY()
		bool bIsAttacking = false;

	UPROPERTY()
		bool bCanAttack = true;

		bool bIsAttackTrace = false;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
		TArray<FWeaponAnimation> LightAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
		TArray<FWeaponAnimation> HeavyAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
		TArray<FWeaponAnimation> AirAttackMontage;

	FVector prevBase;

	FVector prevTip;

	//UPROPERTY(EditDefaultsOnly, Category = "SetUp")
	int32 Damage = 0;
};
