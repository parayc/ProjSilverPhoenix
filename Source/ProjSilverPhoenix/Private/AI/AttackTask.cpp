// Copyright Project Silver Phoneix

#include "AttackTask.h"
#include "MeleeWeapon.h"
#include "SKFootSoldier.h"
#include "AIController.h"
#include "GameFramework/Actor.h"
#include "CombatComponent.h"

EBTNodeResult::Type UAttackTask::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	Super::ExecuteTask(OwnerComp,  NodeMemory);
	bNotifyTick = true;
	auto Controller = OwnerComp.GetAIOwner();
	auto AIPawn = Cast<ASKFootSoldier>(Controller->GetPawn());

	auto Weapon = Cast<AMeleeWeapon>(AIPawn->CharacterEquipment.CurrentWeapon);

	if (Weapon)
	{

		Weapon->SetLastSokcetFrame();
		auto length = Weapon->GetLightAttackMontages().Num();

		auto RanValue = FMath::RandRange(0, length - 1);
		if (length == 0 || Weapon->GetLightAttackMontages()[RanValue].MeleeAttackMontages == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed"))
			return EBTNodeResult::Failed; 
		}
		Duration = 0.f;
		auto AnimationToPlay = Weapon->GetLightAttackMontages()[RanValue].MeleeAttackMontages;
		Duration = Weapon->PlayWeaponAnimation(AnimationToPlay);
		Weapon->SetDamage(Weapon->GetLightAttackMontages()[RanValue].DamagePerAnimation);

		auto CombatComp = AIPawn->GetCombatState();
		if (CombatComp && Weapon->GetLightAttackMontages()[RanValue].SuperArmorThreshold > 0.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Super Armor Active"))
			CombatComp->ActiveSuperArmor(Weapon->GetLightAttackMontages()[RanValue].SuperArmorThreshold);
		}
	}
	
	return EBTNodeResult::InProgress;
	
}


void UAttackTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	Duration -= DeltaSeconds;

	if (Duration <= 0.0f)
	{


		auto Controller = OwnerComp.GetAIOwner();
		auto AIPawn = Cast<ASKFootSoldier>(Controller->GetPawn());
		auto CombatComp = AIPawn->GetCombatState();
		if (CombatComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("Super Armor eNDED"))
			CombatComp->DeactiveSuperArmor();
		}
		
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		
	}
}


