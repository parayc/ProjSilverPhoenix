// Copyright Project Silver Phoneix

#include "EnemyAIController.h"
#include "EnemyMaster.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"



AEnemyAIController::AEnemyAIController()
{

	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));


	Enemy = "Enemy";
}

void AEnemyAIController::Possess(APawn * pawn)
{
	Super::Possess(pawn);

	//Initialize its blackboard and start it behaviour
	if (auto AICharacter = Cast<AEnemyMaster>(pawn))
	{
		if (AICharacter->BehaviorTree->BlackboardAsset != nullptr)
		{
			BlackboardComp->InitializeBlackboard(*(AICharacter->BehaviorTree->BlackboardAsset));
			BehaviorComp->StartTree(*AICharacter->BehaviorTree);
		}
	}

}

void AEnemyAIController::UnPossess()
{
	BehaviorComp->StopTree();
}

void AEnemyAIController::SetSeenTarget(APawn* pawn)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(Enemy, pawn);
	}
}
