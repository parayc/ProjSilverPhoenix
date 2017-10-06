// Copyright Project Silver Phoneix

#include "MeleeWeapon.h"
#include "Particles/ParticleSystemComponent.h"
#include "EnemyMaster.h"
#include "ProjSilverPhonex.h"
#include "XBaseCharacter.h"
#include "DrawDebugHelpers.h"

AMeleeWeapon::AMeleeWeapon()
{
	SwordTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SpawnTrail"));
	SwordTrail->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsAttackTrace)
	{
		AttackTrace();	
	}


}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	if (SwordTrail)
	{
		SwordTrail->BeginTrails(SocketBase, SocketTip, ETrailWidthMode::ETrailWidthMode_FromCentre, 1.f);
		SetSwordTrailVisibility(false);
	}
}

void AMeleeWeapon::StartAttack()
{
	bIsAttacking = true;
}

void AMeleeWeapon::StopAttack()
{
	bIsAttacking = false;
}

void AMeleeWeapon::AttackTrace()
{
	FHitResult HitResult;

	FVector curBase = WeaponMesh->GetSocketLocation(SocketBase), curTip = WeaponMesh->GetSocketLocation(SocketTip);
	const int sub = 20;
	float curLength = (curBase - curTip).Size();
	float prevLength = (prevBase - prevTip).Size();

	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();
	TraceParams->AddIgnoredActor(Instigator);


	for (int i = 1; i < sub; i++)
	{
		FVector tmpBase = FMath::Lerp(curBase, prevBase, i / float(sub));
		FVector tmpTip = FMath::Lerp(curTip, prevTip, i / float(sub));
		FVector tmpOff = (tmpTip - tmpBase);
		tmpOff.Normalize();
		//DrawDebugLine(GetWorld(), tmpBase, tmpBase + tmpOff*FMath::Lerp(curLength, prevLength, i / float(sub)), FColor::Red, false, 1 / 15.0f * 2);
	
		if (GetWorld()->LineTraceSingleByChannel(HitResult, tmpBase, tmpBase + tmpOff*FMath::Lerp(curLength, prevLength, i / float(sub)), ECC_Weapon, *TraceParams))
		{

			AXBaseCharacter* Enemy = Cast<AXBaseCharacter>(HitResult.GetActor());
			//UE_LOG(LogTemp, Warning, TEXT("Enemy: %s"), *HitResult.GetActor()->GetName());
			if (Enemy)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Enemy yh: %s"), *HitResult.GetActor()->GetName());
				//Only add enemy to array if not in array
				if (!EnemiesHit.Contains(Enemy))
				{
					

					//Add to array
					EnemiesHit.Add(Enemy);
					//Deal damage to enemy
					DealDamage(HitResult);

				}
				DrawDebugLine(GetWorld(), tmpBase, tmpBase + tmpOff*FMath::Lerp(curLength, prevLength, i / float(sub)), FColor::Red, false, 1 / 15.0f * 2);
			}

		}
	}
	prevBase = curBase;
	prevTip = curTip;
	//DrawDebugLine(GetWorld(), curBase, curTip, FColor::Green, false, 1 / 15.0f * 2);
}

void AMeleeWeapon::DealDamage(const FHitResult & HitResult)
{
	AXBaseCharacter* Enemy = Cast<AXBaseCharacter>(HitResult.GetActor());
	if (Enemy)
	{
		float DealtDamage = Damage;//Later maybe damage multipler 

		FPointDamageEvent DamageEvent;
		DamageEvent.Damage = DealtDamage;
		DamageEvent.HitInfo = HitResult;
		//TODO - REFACTOR
		Enemy->TakeDamage(DealtDamage, DamageEvent, Instigator->GetController(), MyPawn);
		UE_LOG(LogTemp, Warning, TEXT("Enemy: %s"), *Enemy->GetName());

	}
}

bool AMeleeWeapon::GetIsAttcking()
{
	return bIsAttacking;
}

void AMeleeWeapon::StartTraceAttack()
{
	bIsAttackTrace = true;
	SetSwordTrailVisibility(true);
}

void AMeleeWeapon::StopTraceAttack()
{
	bIsAttackTrace = false;
	SetSwordTrailVisibility(false);
}

void AMeleeWeapon::ClearEnemiesHitArray()
{
	EnemiesHit.Empty();
}

TArray<FWeaponAnimation> AMeleeWeapon::GetLightAttackMontages()
{
	return LightAttackMontage;
}

TArray<FWeaponAnimation> AMeleeWeapon::GetHeavyAttackMontages()
{
	return HeavyAttackMontage;
}

TArray<FWeaponAnimation> AMeleeWeapon::GetAirAttackMontages()
{
	return AirAttackMontage;
}

void AMeleeWeapon::SetSwordTrailVisibility(bool NewState)
{
	if (SwordTrail)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Set Sword trail: %s"), NewState ? TEXT("true") : TEXT("false"));
		SwordTrail->SetVisibility(NewState);
	}

}


