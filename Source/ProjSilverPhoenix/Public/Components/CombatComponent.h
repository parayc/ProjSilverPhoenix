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

	UPROPERTY(EditDefaultsOnly, Category = "CombatComponent")
		float KnockBackAmount = 5;

	void Flinch(const FHitResult& HitResult);

	void StopFlinch();

	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	bool GetIsFlinching() const;

	void KnockDown(AActor* DamageInstigator);

	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	void ResetKnockDown();

	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	void KnockDownEnd();

	void CalculateKnockDown(AActor* DamageInstigator, float KnockDownAmount);

	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	bool GetIsKnockedDown() const;

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

	FTimerHandle KnockDownTimerHandle;

	FTimerHandle KnockDownRecoverTimerHandle;

	float KnockdownThreshold;

	float knockdownValue;

	bool bIsKnockedDown = false;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	
		
	
};
