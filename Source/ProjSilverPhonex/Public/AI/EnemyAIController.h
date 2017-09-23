// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class AActor;

/**
 * 
 */
UCLASS()
class PROJSILVERPHONEX_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
	class UBehaviorTreeComponent* BehaviorComp;

	class UBlackboardComponent* BlackboardComp;

public:

	AEnemyAIController();

	virtual void Possess(APawn* pawn) override;

	virtual void UnPossess() override;

	void SetSeenTarget(AActor* NewEnemy);

	void SetEnemyInRnage(bool NewState);

	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName Enemy;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName EnemyInRange;

	UFUNCTION(BlueprintCallable)
	AActor* GetEnemyRef() const;
	
private:

	AActor* EnemyRef;

};
