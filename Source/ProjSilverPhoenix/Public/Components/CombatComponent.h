// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


class UAnimMontage;


UENUM(BlueprintType)
enum class EBattleState : uint8
{
	PS_SuperArmor UMETA(DisplayName = "SuperArmor"),
	PS_Normal UMETA(DisplayName = "Normal"),
	PS_Invincible UMETA(DisplayName = "Invincible"),
	PS_Unknown UMETA(DisplayName = "Unknown")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJSILVERPHOENIX_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

	EBattleState GetBattleState() const;

	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	void SetBattleState(EBattleState BattleState);

	void KnockBack(AActor * DamageCauser, AActor * DamageReceiver);
	/*How far the actor is hit back*/
	UPROPERTY(EditDefaultsOnly, Category = "CombatComponent")
		float KnockBackAmount = 5;

	void Flinch(const FVector HitDirection);

	void StopFlinch();

	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	bool GetIsFlinching() const;

	void KnockDown(AActor* DamageInstigator);

	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	void ResetKnockDown();

	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	void KnockDownEnd();

	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	bool GetIsKnockedDown() const;

	/*Returns true if the super armor was broken during activation */
	bool CalculateSuperArmor(float Damage);

	void ActiveSuperArmor(float SuperArmorValue);

	void DeactiveSuperArmor();

	void ResetKnockDownValue();

	void HandleDamage(float HealthChange, FVector HitDirection, AActor* PawnInstigator, AActor* ActorHit);

private: 
	FTimerHandle FlinchHandleTimer;

	EBattleState CurrentBattleState;

	bool IsFlinching = false;

	UPROPERTY(EditAnywhere, Category = "CombatComponentAnimation | flinch")
	UAnimMontage* FrontFlinchAnim;

	UPROPERTY(EditAnywhere, Category = "CombatComponentAnimation | flinch")
	UAnimMontage* BackFlinchAnim;

	UPROPERTY(EditAnywhere, Category = "CombatComponentAnimation | flinch")
	UAnimMontage* LeftFlinchAnim;

	UPROPERTY(EditAnywhere, Category = "CombatComponentAnimation | flinch")
	UAnimMontage* RightFlinchAnim;

	UPROPERTY(EditAnywhere, Category = "CombatComponentAnimation | KnockDown")
		UAnimMontage* KnockDownAnim;

	UPROPERTY(EditAnywhere, Category = "CombatComponentAnimation | KnockDown")
		UAnimMontage* KnockDownRecoveryAnim;

	FTimerHandle KnockDownLastHitTimerHandle;

	FTimerHandle KnockDownRecoverTimerHandle;
	

	UPROPERTY(EditAnywhere, Category = "CombatComponent")
	bool bCanBeKnockdown = true;
	/*The amount of damage needed before the actor is knocked down*/
	UPROPERTY(EditAnywhere, Category = "CombatComponent", meta = (EditCondition = "bCanBeKnockdown"))
	float KnockdownThreshold = 25;
	/*This is how long it would take to reset the knock down value */
	UPROPERTY(EditAnywhere, Category = "CombatComponent", meta = (EditCondition = "bCanBeKnockdown"))
	float LastTimeHit = 2;

	float CurrentknockdownValue;

	/*The amount of damage needed before the actor super amror is broken - Set in Attack Task*/
	float SuperArmorThreshold;

	float CurrentSuperArmorValue = 0;

	bool bIsKnockedDown = false;

	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	
		
	
};
