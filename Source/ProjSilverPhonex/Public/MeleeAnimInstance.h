// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MeleeAnimInstance.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	PS_Light UMETA(DisplayName = "Light"),
	PS_Heavy UMETA(DisplayName = "Heavy"),
	PS_Air UMETA(DisplayName = "Air")

};

UCLASS()
class PROJSILVERPHONEX_API UMeleeAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:


	void Attack(EAttackType AttackType);

	void PlayCombo(EAttackType AttackType);
	
	void ComboReset();

	void Reset();

	//Resets the array hit list a
	UFUNCTION(BlueprintCallable, Category = "WeaponSetup")
	void StopAttack();

	void ChangeDirection();

	/*Called in the anim instance using BP notifies - this stops the line trace  */
	UFUNCTION(BlueprintCallable, Category = "WeaponSetup")
	void StopAttackingTrace();
	/*Called in the anim instance using BP notifies - this starts the line trace  */
	UFUNCTION(BlueprintCallable, Category = "WeaponSetup")
	void StartAttackingTrace();

	void SetAcceptNextCombo(bool NewState);

	float PlayAnimation(UAnimMontage* Animation, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	EAttackType AttackTypes;

private:

	FTimerHandle ComboTimerHandle;

	int ComboCounter = 0;

	bool bisAttacking = false;

	bool bAcceptNextCombo = false;

	float MoveForwad = 0.f;

	float MoveRight = 0.f;
};
