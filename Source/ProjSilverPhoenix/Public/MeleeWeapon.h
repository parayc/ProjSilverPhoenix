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
class USoundCue;

USTRUCT(BlueprintType)
struct FWeaponAnimation
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetUp")
	class UAnimMontage* MeleeAttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetUp")
		int32 DamagePerAnimation = 0;
	//Some Montages 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetUp", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float KnockDownAmount;
	/*If value is zero the montage has no Super Armor - The value is the amount of damage it takes to break the super armor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetUp")
	float SuperArmorThreshold = 0.f;

};
UCLASS()
class PROJSILVERPHOENIX_API AMeleeWeapon : public ABaseWeapon
{
	GENERATED_BODY()


	AMeleeWeapon();

public:

	virtual void Tick(float DeltaTime) override;

	virtual void StartAttack() override;

	virtual void StopAttack() override;

	virtual void PressFocus() override;

	virtual void ReleaseFocus() override;

	void SetDamage(int32 Value);

	void DealDamage(const FHitResult& HitResult, TSubclassOf<UDamageType> DamageType);

	void HandleDamage(const FHitResult& HitResult, TSubclassOf<UDamageType> DamageType);

	bool CanDamage(const FHitResult& HitResult);
	
	UFUNCTION(BlueprintCallable, Category = "Setup")
	bool GetIsAttcking() const;

	UFUNCTION(BlueprintCallable, Category = "Setup")
	bool GetIsTracing() const;

	void SetLastSokcetFrame();

	//Starts weapons line trace 
	void StartTraceAttack();

	//Ends weapons line trace 
	void StopTraceAttack();

	//Clears array
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void ClearEnemiesHitArray();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	float GetDamageModifier() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetDamageModifier(float newDamage);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void GroundSlamAttack();

	void GroundSlamReset(AActor* HitActor);

	TArray<FWeaponAnimation> GetLightAttackMontages();

	TArray<FWeaponAnimation> GetHeavyAttackMontages();

	TArray<FWeaponAnimation> GetAirAttackMontages();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FX")
	UParticleSystemComponent* SwordTrail;

	void TraceSwing();

	void PlaySound(USoundBase * Sound);

	void SpawnHitEffext(FHitResult& Hit);

	

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:


	bool IsTargetWithinSight(AActor* Target);

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
		USoundBase* SwordImpactSounds;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USoundBase* GroundSlamSounds;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* HitFX;

	TArray<AActor*> EnemiesHit;

	UPROPERTY(EditDefaultsOnly, Category = "Melee Weapon")
	float DamageModifier = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Melee Weapon")
	float GroundSlamAttackRadius = 350.f;

	UPROPERTY(EditDefaultsOnly, Category = "Melee Weapon")
		FName SocketBase;

	UPROPERTY(EditDefaultsOnly, Category = "Melee Weapon")
		FName SocketTip;

	/* whether to draws the ray cast lines of the sword trace and ground slam*/
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
		bool bDrawDebugLines = false;

	/* how many traces to draw on the sword  per swing - More traces increase accuracy*/
	UPROPERTY(EditAnywhere, Category = "Debug")
		float AmountToTrace = 5;

	UPROPERTY()
		bool bCanAttack = true;

		bool bIsAttackTrace = false;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		TArray<FWeaponAnimation> LightAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TArray<FWeaponAnimation> HeavyAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TArray<FWeaponAnimation> AirAttackMontage;

	FVector prevBase;

	FVector prevTip;

	/*These hold the location of the sockets on the previous frame*/
	FVector LastFrameStartSocket;
	FVector LastFrameEndSocket;

	/*The location of the sockets when the swing begun*/
	FVector StartSocket;
	FVector EndSocket;

	//UPROPERTY(EditDefaultsOnly, Category = "SetUp")
	int32 Damage = 0;

	UPROPERTY()
	bool bIsAttacking = false;

};
