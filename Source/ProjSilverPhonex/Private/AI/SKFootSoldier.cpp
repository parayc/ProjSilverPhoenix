// Copyright Project Silver Phoneix

#include "SKFootSoldier.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "SPlayer.h"
#include "EnemyAIController.h"
#include "BaseWeapon.h"
#include "MeleeWeapon.h"


ASKFootSoldier::ASKFootSoldier()
{

	BattleCircle = CreateDefaultSubobject<USphereComponent>(TEXT("BattleCircle"));
	BattleCircle->SetupAttachment(RootComponent);
}

void ASKFootSoldier::SetIsAttacking(bool NewState)
{
	bIsAttacking = NewState;
}

void ASKFootSoldier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ASKFootSoldier::BeginPlay()
{
	Super::BeginPlay();

		if (BattleCircle)
		{
			
			BattleCircle->OnComponentBeginOverlap.AddDynamic(this, &ASKFootSoldier::OnEnemyEnterDamageBox);
			BattleCircle->OnComponentEndOverlap.AddDynamic(this, &ASKFootSoldier::OnEnemyLeavesDamageBox);
		}
		
}


void ASKFootSoldier::OnEnemyEnterDamageBox(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFomSweep, const FHitResult & SweepResult)
{
	if (auto Player = Cast<ASPlayer>(OtherActor))
	{
		if (auto AIController = Cast<AEnemyAIController>(GetController()))
		{
			bEnemyInRange = true;
			AIController->SetEnemyInRnage(true);
			
		}
	}
}

void ASKFootSoldier::OnEnemyLeavesDamageBox(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (auto Player = Cast<ASPlayer >(OtherActor))
	{
		if (auto AIController = Cast<AEnemyAIController>(GetController()))
		{
			bEnemyInRange = false;
			AIController->SetEnemyInRnage(false);
		}
	}
}


void ASKFootSoldier::ResetEnemyAttackList()
{
	EnemiesHitList.Empty();
}
