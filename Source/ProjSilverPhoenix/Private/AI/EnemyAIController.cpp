// Copyright Project Silver Phoneix

#include "EnemyAIController.h"
#include "EnemyMaster.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"



AEnemyAIController::AEnemyAIController()
{

	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));


	Enemy = "Enemy";
	EnemyInRange = "EnemyInRange";
	NoiseLocation = "TargetDestination";
	AIState = "AIState";
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

void AEnemyAIController::SetAIState(EAIStates newState)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Enum>(AIState, static_cast<UBlackboardKeyType_Enum::FDataType>(newState));
	}
}

AActor * AEnemyAIController::GetEnemyRef() const
{
	return EnemyRef;
}

