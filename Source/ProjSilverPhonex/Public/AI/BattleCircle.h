// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BattleCircle.generated.h"

/**
 * 
 */
UCLASS()
class PROJSILVERPHONEX_API UBattleCircle : public UBTTaskNode
{
	GENERATED_BODY()
	
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds) override;

	float Duration;

	FVector Direction;

	int32 RandomNumber;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector Enemy;
	
};
