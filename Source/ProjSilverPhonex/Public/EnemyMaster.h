// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyMaster.generated.h"


class UWidgetComponent;
class UStaticMeshComponent;
class UPawnSensingComponent;
class UBehaviorTree;

UCLASS()
class PROJSILVERPHONEX_API AEnemyMaster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyMaster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Applies damage to the character */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere, Category = "Setup")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* TargetIcon;

	UPROPERTY(VisibleAnywhere, Category = "Setup")
	UPawnSensingComponent* PawningSensingComp;

	bool GetIsDead();

	void OnDeath();

	void SetTargetIconDirection();

	void SetTargetHidden(bool NewState);

	bool GetbIsTargeted();

private:

	UFUNCTION()
	void OnseePlayer(APawn* pawn);

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	int32 MaxHealth = 100;

	UPROPERTY(VisibleAnywhere, Category = "Stats")
	int32 Health = 0;

	int32 AttackDamage;

	bool bIsTargeted = false;

	bool bIsDead = false;

};
