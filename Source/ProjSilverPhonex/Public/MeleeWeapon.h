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

	void SetDamage(int32 Value);

	void DealDamage(const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, Category = "Setup")
	bool GetIsAttcking();

	void SetLastSokcetFrame();

	//Starts weapons line trace 
	void StartTraceAttack();

	//Ends weapons line trace 
	void StopTraceAttack();

	//Clears array
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void ClearEnemiesHitArray();

	UFUNCTION(BlueprintCallable, Category = "Setup")
	float GetDamageModifier() const;

	TArray<FWeaponAnimation> GetLightAttackMontages();

	TArray<FWeaponAnimation> GetHeavyAttackMontages();

	TArray<FWeaponAnimation> GetAirAttackMontages();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FX")
	UParticleSystemComponent* SwordTrail;

	void TraceSwing();

	void PlaySound(USoundBase * Sound);

	void SpawnHitEffext(FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
		UParticleSystem* HitFX;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* SwordImpactSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* SwordImpactSounds;

	TArray<AXBaseCharacter*> EnemiesHit;
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
		float DamageModifier = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
		FName SocketBase;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
		FName SocketTip;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
		bool bDrawDebugLines = false;

	UPROPERTY()
		bool bIsAttacking = false;

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

	/* how many traces this weapon will do per swing*/
	UPROPERTY(EditAnywhere, Category = Trace)
	float AmountToTrace = 5;


	

	//UPROPERTY(EditDefaultsOnly, Category = "SetUp")
	int32 Damage = 0;
};
