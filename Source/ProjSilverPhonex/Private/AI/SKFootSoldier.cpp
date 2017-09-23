// Copyright Project Silver Phoneix

#include "SKFootSoldier.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "BasePlayer.h"
#include "EnemyAIController.h"

void ASKFootSoldier::SetIsAttacking(bool NewState)
{
	bIsAttacking = NewState;
}

void ASKFootSoldier::BeginPlay()
{
	Super::BeginPlay();

		if (Weapon && DamageBox && BattleCircle)
		{
			Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
			DamageBox->OnComponentBeginOverlap.AddDynamic(this, &ASKFootSoldier::OnPlayerEnterPickupBox);
			BattleCircle->OnComponentBeginOverlap.AddDynamic(this, &ASKFootSoldier::OnEnemyEnterDamageBox);
			BattleCircle->OnComponentEndOverlap.AddDynamic(this, &ASKFootSoldier::OnEnemyLeavesDamageBox);
		}
		
}

void ASKFootSoldier::OnPlayerEnterPickupBox(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFomSweep, const FHitResult & SweepResult)
{

	//UE_LOG(LogTemp, Warning, TEXT("Player enter sword box"))
	//UE_LOG(LogTemp, Warning, TEXT("Actor: %s"), *OtherActor->GetName())
	if (bIsAttacking)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Actor: %s"), *OtherActor->GetName())
		
		
			if (auto Player = Cast<ABasePlayer>(OtherActor))
			{
				if (!EnemiesHitList.Contains(OtherActor))
				{
					UE_LOG(LogTemp, Warning, TEXT("Damage player"))
					float DealtDamage = WeaponDamage;//Later maybe damage multipler 

					FPointDamageEvent DamageEvent;
					DamageEvent.Damage = DealtDamage;
					Player->TakeDamage(DealtDamage, DamageEvent, GetController(), this);
					EnemiesHitList.Add(OtherActor);

				}
			}
	
	}
}

void ASKFootSoldier::OnEnemyEnterDamageBox(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFomSweep, const FHitResult & SweepResult)
{
	if (auto Player = Cast<ABasePlayer>(OtherActor))
	{
		
		if (auto AIController = Cast<AEnemyAIController>(GetController()))
		{
			AIController->SetEnemyInRnage(true);
		}
	}
}

void ASKFootSoldier::OnEnemyLeavesDamageBox(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (auto Player = Cast<ABasePlayer>(OtherActor))
	{

		if (auto AIController = Cast<AEnemyAIController>(GetController()))
		{
			AIController->SetEnemyInRnage(false);
		}
	}
}

ASKFootSoldier::ASKFootSoldier()
{

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	Weapon->SetupAttachment(RootComponent);

	DamageBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Damage Box"));
	DamageBox->AttachToComponent(Weapon, FAttachmentTransformRules::KeepRelativeTransform);

	BattleCircle = CreateDefaultSubobject<USphereComponent>(TEXT("BattleCircle"));
	BattleCircle->SetupAttachment(RootComponent);
}

void ASKFootSoldier::ResetEnemyAttackList()
{
	EnemiesHitList.Empty();
}
