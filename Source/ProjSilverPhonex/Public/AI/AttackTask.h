// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AttackTask.generated.h"

/**
 * 
 */

class ASKFootSoldier;
UCLASS()
class PROJSILVERPHONEX_API UAttackTask : public UBTTaskNode
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


	virtual void TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds) override;



	float dur = 0.0f;
public:
	//UPROPERTY(EditA)
	
};
