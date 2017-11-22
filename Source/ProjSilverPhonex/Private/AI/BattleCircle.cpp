// Copyright Project Silver Phoneix - Use blueprint version called StrafeTask as its the exact same as c++ class but the radnom direction and duration works in BP version

#include "BattleCircle.h"
#include "AIController.h"
#include "XBaseCharacter.h"

EBTNodeResult::Type UBattleCircle::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	bNotifyTick = true;

	Duration = FMath::RandRange(0, 8);
	RandomNumber = FMath::RandRange(0, 1);

	auto AIPawn = Cast<AXBaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (AIPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI: %s - Duration: %f"), *AIPawn->GetName(), Duration);
	}
	

	return EBTNodeResult::InProgress;
}

void UBattleCircle::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{

	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	auto AIPawn = Cast<AXBaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (AIPawn)
	{
		if (!AIPawn->IsFlinching())
		{
			if (RandomNumber == 1)
			{
				Direction = OwnerComp.GetAIOwner()->GetPawn()->GetActorRightVector();
			}
			else
			{
				Direction = OwnerComp.GetAIOwner()->GetPawn()->GetActorRightVector() * -1;
			}


			AIPawn->AddMovementInput(Direction, 1);
		}
	}

	Duration -= DeltaSeconds;
	//UE_LOG(LogTemp, Warning, TEXT("AI: %s - Duration: %f"), *AIPawn->GetName(), Duration);
	if (Duration <= 0)
	{

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

}
