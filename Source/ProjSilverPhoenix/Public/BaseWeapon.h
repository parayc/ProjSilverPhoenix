// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "BaseWeapon.generated.h"

class AXBaseCharacter;

UCLASS()
class PROJSILVERPHOENIX_API ABaseWeapon : public ABaseItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float NoEquipAnimDuration = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* UnEquipAnimation;

	void SetOwningPawn(AXBaseCharacter* NewOwner);

	UFUNCTION(BlueprintCallable,Category = "Animation")
	float PlayWeaponAnimation(UAnimMontage* Animation, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	virtual void StartAttack() {};

	virtual void StopAttack() {};

	virtual void ReleaseAttack() {};

	virtual void PressAttack() {};

	virtual void PressFocus() {};

	virtual void ReleaseFocus() {};

	virtual void UseItem(AActor* OwnerController) override;

	void UnEquip();

	void EquipingFinished();

	void DestroyWeapon();

	/*The Socket will connect to the owner body*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName BackSocketName;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/** pawn owner */
	UPROPERTY(BlueprintReadOnly)
	class AXBaseCharacter* MyPawn;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UDamageType> PrimaryDamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UDamageType> SecondaryDamageType;

private:

	FTimerHandle EquipFinishedTimerHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
