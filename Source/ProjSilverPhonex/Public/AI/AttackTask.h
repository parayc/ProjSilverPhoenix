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
	
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	void EndAttack();

	FTimerHandle AttackTimerHandle;
public:
	//UPROPERTY(EditA)
	
};
