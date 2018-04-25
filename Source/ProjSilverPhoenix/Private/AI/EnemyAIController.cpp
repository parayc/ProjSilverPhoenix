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
	EnemyInRange = "EnemyInRange";
	NoiseLocation = "NoiseLocation";
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

void AEnemyAIController::SetSeenTarget(AActor* NewEnemy)
{
	
	if (BlackboardComp && NewEnemy != EnemyRef)
	{
		EnemyRef = NewEnemy;
		BlackboardComp->SetValueAsObject(Enemy, EnemyRef);

	}
}

void AEnemyAIController::SetHeardLocation(FVector Location)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsVector(NoiseLocation, Location);
		

	}
}

void AEnemyAIController::SetEnemyInRnage(bool NewState)
{

	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsBool(EnemyInRange, NewState);
	}
}

AActor * AEnemyAIController::GetEnemyRef() const
{
	return EnemyRef;
}

