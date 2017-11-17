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
class PROJSILVERPHONEX_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

	EBattleState GetBattleState() const;

	void SetBattleState(EBattleState BattleState);

	void KnockBack(ACharacter* DamageCauser, ACharacter* DamageReceiver);

	UPROPERTY(EditDefaultsOnly, Category = "SetUp")
		float KnockBackAmount = 5;


	//Play flinch 
	void Flinch();

	void StopFlinch();

	bool GetIsFlinching() const;

	

private: 
	FTimerHandle FlinchHandleTimer;

	EBattleState CurrentBattleState;

	bool IsFlinching = false;

	UPROPERTY(EditAnywhere, Category = "SetUp")
		UAnimMontage* FlinchAnimation;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	
		
	
};
