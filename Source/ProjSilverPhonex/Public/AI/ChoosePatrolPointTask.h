// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ChoosePatrolPointTask.generated.h"

/**
 * 
 */
UCLASS()
class PROJSILVERPHONEX_API UChoosePatrolPointTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
	UPROPERTY(EditAnywhere, Category = "Keys")
	struct FBlackboardKeySelector KeyIndex;
	
	UPROPERTY(EditAnywhere, Category = "Keys")
		struct FBlackboardKeySelector WaypointKey;
};
