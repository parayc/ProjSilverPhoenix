// Copyright Project Silver Phoneix

#include "ChoosePatrolPointTask.h"
#include "PatrolComponent.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UChoosePatrolPointTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{



	auto ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	auto PatrolRoute = ControlledPawn->FindComponentByClass<UPatrolComponent>();
	if (!ensure(PatrolRoute)) { return EBTNodeResult::Failed; }

	//Get Array of patrol points
	auto PatrolPoints = PatrolRoute->GetPatrolPoints();

	//check is patrol points is empety
	if (PatrolPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI: %s has no patrol point"), *ControlledPawn->GetName());
		return EBTNodeResult::Failed;

	}

	//Set Next waypoint
	auto BlackBoardComp = OwnerComp.GetBlackboardComponent();
	auto Index = BlackBoardComp->GetValueAsInt(KeyIndex.SelectedKeyName);//Gets the blackboard key name
	BlackBoardComp->SetValueAsObject(WaypointKey.SelectedKeyName, PatrolPoints[Index]);

	//Cycle the index
	auto NextIndex = (Index + 1) % PatrolPoints.Num();
	BlackBoardComp->SetValueAsInt(KeyIndex.SelectedKeyName, NextIndex);

	return EBTNodeResult::Succeeded;

}




