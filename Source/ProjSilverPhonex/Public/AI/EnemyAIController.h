// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class APawn;

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

	void SetSeenTarget(APawn* pawn);

	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName Enemy;

};
