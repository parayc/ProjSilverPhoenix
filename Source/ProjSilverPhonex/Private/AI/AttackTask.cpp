// Copyright Project Silver Phoneix

#include "AttackTask.h"
#include "MeleeWeapon.h"
#include "SKFootSoldier.h"
#include "AIController.h"
#include "GameFramework/Actor.h"

EBTNodeResult::Type UAttackTask::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	Super::ExecuteTask(OwnerComp,  NodeMemory);
	bNotifyTick = true;
	auto Controller = OwnerComp.GetAIOwner();
	auto AIPawn = Cast<ASKFootSoldier>(Controller->GetPawn());

	auto Weapon = Cast<AMeleeWeapon>(AIPawn->CharacterEquipment.CurrentWeapon);

	if (Weapon)
	{
		auto length = Weapon->GetLightAttackMontages().Num();

		auto RanValue = FMath::RandRange(0, length - 1);
		if (Weapon->GetLightAttackMontages()[RanValue].MeleeAttackMontages == nullptr)
		{
			return EBTNodeResult::Failed; 
		}

		auto AnimationToPlay = Weapon->GetLightAttackMontages()[RanValue].MeleeAttackMontages;
		dur = Weapon->PlayWeaponAnimation(AnimationToPlay);

		
	}
	UE_LOG(LogTemp, Warning, TEXT("In Progress"));
	return EBTNodeResult::InProgress;
	
}


void UAttackTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	UE_LOG(LogTemp, Warning, TEXT("tick"));
	dur -= DeltaSeconds;
	if (dur <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Done"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

//
//EBTNodeResult::Type UAttackTask::FinishAttack
//{
//	return EBTNodeResult::Succeeded();
//}
//
