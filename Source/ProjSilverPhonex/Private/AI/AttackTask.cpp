// Copyright Project Silver Phoneix

#include "AttackTask.h"
#include "MeleeWeapon.h"
#include "SKFootSoldier.h"
#include "AIController.h"

EBTNodeResult::Type UAttackTask::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{


	auto Controller = OwnerComp.GetAIOwner();
	auto AIPawn = Cast<ASKFootSoldier>(Controller->GetPawn());

	auto Weapon = Cast<AMeleeWeapon>(AIPawn->CharacterEquipment.CurrentWeapon);

	if (Weapon)
	{
		auto AnimationToPlay = Weapon->GetLightAttackMontages()[0].MeleeAttackMontages;
		Weapon->PlayWeaponAnimation(AnimationToPlay);
	}

	return EBTNodeResult::Succeeded;
	
}

