// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

class ABasePlayer;

UCLASS()
class PROJSILVERPHONEX_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly)
	FName WeaponSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float NoEquipAnimDuration = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* UnEquipAnimation;

	void SetOwningPawn(ABasePlayer* NewOwner);

	float PlayWeaponAnimation(UAnimMontage* Animation, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	virtual void StartAttack();

	virtual void StopAttack();

	void UnEquip();

	void EquipingFinished();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/** pawn owner */
	UPROPERTY()
	class ABasePlayer* MyPawn;

private:

	FTimerHandle EquipFinishedTimerHandle;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
