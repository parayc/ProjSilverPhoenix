// Copyright Project Silver Phoneix

#include "MeleeWeapon.h"
#include "Particles/ParticleSystemComponent.h"
#include "EnemyMaster.h"
#include "ProjSilverPhonex.h"
#include "XBaseCharacter.h"
#include "DrawDebugHelpers.h"
#include "MeleeAnimInstance.h"

AMeleeWeapon::AMeleeWeapon()
{
	SwordTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SpawnTrail"));
	SwordTrail->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);


	TraceParams = new FCollisionQueryParams();
}

void AMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsAttackTrace)
	{
		TraceSwing();
	}


}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	//Crashes if the socketbase and sockettip is not set
	if (SwordTrail)
	{
		SwordTrail->BeginTrails(SocketBase, SocketTip, ETrailWidthMode::ETrailWidthMode_FromCentre, 1.f);
		SetSwordTrailVisibility(false);
	}
}

void AMeleeWeapon::StartAttack()
{

	UMeleeAnimInstance* AnimInstance = Cast<UMeleeAnimInstance>(MyPawn->GetMesh()->GetAnimInstance());
	AnimInstance->Attack(EAttackType::PS_Light);

	SetLastSokcetFrame();

}

void AMeleeWeapon::StopAttack()
{
	bIsAttacking = false;
}

void AMeleeWeapon::TraceSwing()
{

	FHitResult HitResult;
	TraceParams->AddIgnoredActor(Instigator);
	FVector TraceStart, TraceEnd;


	StartSocket = WeaponMesh->GetSocketLocation(SocketBase);
	EndSocket = WeaponMesh->GetSocketLocation(SocketTip);

	for (int i = 0; i < AmountToTrace; i++)
	{

		TraceStart = FMath::Lerp(StartSocket, EndSocket, i / AmountToTrace);
		TraceEnd = FMath::Lerp(LastFrameStartSocket, LastFrameEndSocket, i / AmountToTrace);

		if (bDrawDebugLines)
		{
			DrawDebugLine(Instigator->GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 5, 0, 2.0);
		}
		

		if (GetWorld()->LineTraceSingleByChannel(HitResult,TraceStart,TraceEnd,ECC_Weapon,TraceParams))
		{
		

			AXBaseCharacter* Enemy = Cast<AXBaseCharacter>(HitResult.GetActor());
			//UE_LOG(LogTemp, Warning, TEXT("Enemy: %s"), *HitResult.GetActor()->GetName());
			if (Enemy)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Enemy yh: %s"), *HitResult.GetActor()->GetName());
				//Only add enemy to array if not in array
				if (!EnemiesHit.Contains(Enemy))
				{

					
					EnemiesHit.Add(Enemy);
					//Deal damage to enemy that was added
					DealDamage(HitResult);

				}
			
			}
		}

	}

	/* Store the location of the previous frame*/
	LastFrameStartSocket = StartSocket;
	LastFrameEndSocket = EndSocket;
}



void AMeleeWeapon::SetDamage(int32 Value)
{
	Damage = Value;
}

void AMeleeWeapon::DealDamage(const FHitResult & HitResult)
{
	AXBaseCharacter* Enemy = Cast<AXBaseCharacter>(HitResult.GetActor());

	if (Enemy)
	{
		if (Enemy->GetTeamNumber() != MyPawn->GetTeamNumber())
		{
			float DealtDamage = Damage;//Later maybe damage multipler 

			FPointDamageEvent DamageEvent;
			DamageEvent.Damage = DealtDamage;
			DamageEvent.HitInfo = HitResult;

			Enemy->TakeDamage(DealtDamage, DamageEvent, Instigator->GetController(), MyPawn);
			//UE_LOG(LogTemp, Warning, TEXT("Enemy: %s"), *Enemy->GetName());
		}
	
	}
}

bool AMeleeWeapon::GetIsAttcking()
{
	return bIsAttacking;
}

void AMeleeWeapon::SetLastSokcetFrame()
{
	LastFrameStartSocket = WeaponMesh->GetSocketLocation(SocketBase);
	LastFrameEndSocket = EndSocket = WeaponMesh->GetSocketLocation(SocketTip);
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



