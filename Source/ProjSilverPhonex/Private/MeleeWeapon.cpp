// Copyright Project Silver Phoneix

#include "MeleeWeapon.h"
#include "Particles/ParticleSystemComponent.h"
#include "EnemyMaster.h"
#include "ProjSilverPhonex.h"
#include "XBaseCharacter.h"
#include "DrawDebugHelpers.h"
#include "MeleeAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/DestructibleActor.h"

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
		TraceSwing();
	}


}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	

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
	//This doesnt work 
	FCollisionQueryParams* TraceParams = new FCollisionQueryParams(FName(""),false, Instigator);
	TraceParams->AddIgnoredActor(MyPawn);

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
		

		if (GetWorld()->LineTraceSingleByChannel(HitResult,TraceStart,TraceEnd,ECC_Weapon, TraceParams))
		{

			AXBaseCharacter* Enemy = Cast<AXBaseCharacter>(HitResult.GetActor());
			//UE_LOG(LogTemp, Warning, TEXT("Enemy: %s"), *HitResult.GetActor()->GetName());
			if (Enemy)
			{
			
				if (!EnemiesHit.Contains(Enemy) && (Enemy != MyPawn) && !Enemy->GetIsDead())
				{
					EnemiesHit.Add(Enemy);
					//Deal damage to enemy that was added
					DealDamage(HitResult);

					//UE_LOG(LogTemp, Warning, TEXT("Enemy: %s CurrentHealth: %f"), *HitResult.GetActor()->GetName(), Enemy->GetCurrentHealth());
					if (HitResult.bBlockingHit)
					{
						SpawnHitEffext(HitResult);
						PlaySound(SwordImpactSounds);
					}
				}
			
			}
			else if(ADestructibleActor* DA = Cast<ADestructibleActor>(HitResult.GetActor()))
			{

				UE_LOG(LogTemp, Warning, TEXT("Hit box"));
				FVector EyeLocation;
				FRotator EyeRotation;
				MyPawn->GetActorEyesViewPoint(EyeLocation, EyeRotation);

				FVector AttackDirection = EyeRotation.Vector();
				TArray<AActor*> AllActors;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), AXBaseCharacter::StaticClass(), AllActors);
				UGameplayStatics::ApplyRadialDamage(GetWorld(), 10.f, HitResult.GetActor()->GetActorLocation(), 30.f, DamageType, AllActors, this, MyPawn->GetInstigatorController(), ECC_Weapon);
				
			}
		}

	}

	/* Store the location of the previous frame*/
	LastFrameStartSocket = StartSocket;
	LastFrameEndSocket = EndSocket;
}

void AMeleeWeapon::PlaySound(USoundBase * Sound)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(),Sound,GetActorLocation());
	
}

void AMeleeWeapon::SpawnHitEffext(FHitResult& Hit)
{
	FVector Location = Hit.ImpactPoint;
	FRotator Rotation = Hit.ImpactPoint.Rotation();
	if (HitFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitFX, Location, Rotation, true);
	}
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
			float DealtDamage = Damage * DamageModifier;

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
	if (SwordTrail)
	{
		SwordTrail->BeginTrails(SocketBase, SocketTip, ETrailWidthMode::ETrailWidthMode_FromCentre, 1.f);
	}
}

void AMeleeWeapon::StopTraceAttack()
{
	bIsAttackTrace = false;

	if (SwordTrail)
	{
		SwordTrail->EndTrails();
	}
}

void AMeleeWeapon::ClearEnemiesHitArray()
{
	EnemiesHit.Empty();
}

float AMeleeWeapon::GetDamageModifier() const
{
	return DamageModifier;
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





