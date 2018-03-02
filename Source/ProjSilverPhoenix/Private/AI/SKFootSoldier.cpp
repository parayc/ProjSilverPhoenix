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

	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("BattleCircle"));
	AttackSphere->SetupAttachment(RootComponent);
}

void ASKFootSoldier::SetIsAttacking(bool NewState)
{
	bIsAttacking = NewState;
}

void ASKFootSoldier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetIsDead())
	{
		float amount = GetWorld()->TimeSeconds - TimeSinceLastDeath;

		float DissolveAmount = amount / DissolveLength;
		if (PrimeMatInst)
		{
			PrimeMatInst->SetScalarParameterValue("Amount", DissolveAmount);
		}

		if (SecondaryMatInst)
		{
			SecondaryMatInst->SetScalarParameterValue("Amount", DissolveAmount);
		}
	}

	
}



void ASKFootSoldier::BeginPlay()
{
	Super::BeginPlay();

	PrimeMatInst = GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(0, GetMesh()->GetMaterial(0));
	SecondaryMatInst = GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(1, GetMesh()->GetMaterial(1));

	if (AttackSphere)
	{
			
			AttackSphere->OnComponentBeginOverlap.AddDynamic(this, &ASKFootSoldier::OnEnemyEnterDamageBox);
			AttackSphere->OnComponentEndOverlap.AddDynamic(this, &ASKFootSoldier::OnEnemyLeavesDamageBox);
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
