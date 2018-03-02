// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "XBaseCharacter.h"
#include "EnemyMaster.generated.h"


class UWidgetComponent;
class UStaticMeshComponent;
class UPawnSensingComponent;
class UBehaviorTree;
class UAnimMontage;
//class UCombatComponent;

UCLASS()
class PROJSILVERPHOENIX_API AEnemyMaster : public AXBaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyMaster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float TimeSinceLastDeath;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Setup")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* TargetIcon;

	UPROPERTY(VisibleAnywhere, Category = "Setup")
	UPawnSensingComponent* PawnSensingComp;

	FTimerHandle DeathTimerHandle;
	virtual void OnDeath() override;

	void SetTargetIconVisibility(bool NewState);

	

private:

	//UPROPERTY(Category = "SetUp", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//	UCombatComponent* CombatStates = nullptr;

	UFUNCTION()
	void OnseePlayer(APawn* pawn);

	UFUNCTION()
		void OnHearNoise(APawn * PawnInstigator, const FVector & Location, float Volume);

	int32 AttackDamage;

	bool bSensedTarget = false;

	bool bIsTargeted = false;

	float LastSeenTime;

	/*This is how long the AI will follow the enemy when there are not in their line of sight */
	UPROPERTY(EditDefaultsOnly, Category = "SetUp")
	float SenseTimeOut;

	/*This is how much exp the player while receive when the AI is killed */
	UPROPERTY(EditDefaultsOnly, Category = "SetUp")
	int32 Exp;


};
