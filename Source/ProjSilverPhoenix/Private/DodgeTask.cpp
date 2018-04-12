// Copyright Project Silver Phoneix

#include "DodgeTask.h"
#include "SKFootSoldier.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UDodgeTask::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	


	auto Controller = OwnerComp.GetAIOwner();
	if (!Controller) {  return EBTNodeResult::Failed; }
	auto PawnOnwer = Controller->GetPawn();


	auto Target = OwnerComp.GetBlackboardComponent()->GetValueAsObject(EnemyKey.SelectedKeyName);
	auto Enemy = Cast<AActor>(Target);
	
	if (!Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed"))
		return EBTNodeResult::Failed;
	}

	auto startLocation = Enemy->GetActorLocation();
	auto DirectionToTarget = (PawnOnwer->GetActorLocation() - startLocation).GetSafeNormal();
	auto Perp = DirectionToTarget.RotateAngleAxis(90, FVector(0, 0, 1));

	
	//do dodge left or right 
	//Perp *= FMath::RandRange(0, 1) ? 1 : -1;

	auto GoalDestination = startLocation + (Perp * 500);
	//Line trace to
	FHitResult Hit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(PawnOnwer);
	if (GetWorld()->LineTraceSingleByChannel(Hit, startLocation, GoalDestination, ECollisionChannel::ECC_Visibility, TraceParams))
	{
		//Move to location
		Controller->MoveToLocation(Hit.Location, 0);
		UE_LOG(LogTemp, Warning, TEXT("Dodge"))

	}
	Controller->MoveToLocation(GoalDestination, 0);

	return EBTNodeResult::InProgress;
}

void UDodgeTask::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	UE_LOG(LogTemp, Warning, TEXT("Dodge complete"))
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
