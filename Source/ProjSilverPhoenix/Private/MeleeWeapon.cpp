// Copyright Project Silver Phoneix

#include "MeleeWeapon.h"
#include "Particles/ParticleSystemComponent.h"
#include "EnemyMaster.h"
#include "ProjSilverPhoenix.h"
#include "XBaseCharacter.h"
#include "DrawDebugHelpers.h"
#include "MeleeAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/DestructibleActor.h"
#include "CombatComponent.h"

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
			
			//Check is the actor we hit is alive
			if (Enemy && !Enemy->GetIsDead())
			{

				//We check if we hit the enemy already in the same swing and not hit ourself
				if (!EnemiesHit.Contains(Enemy) && (Enemy != MyPawn) )
				{
					EnemiesHit.Add(Enemy);
					//Deal damage to enemy that was added
					DealDamage(HitResult);

					
				
					SpawnHitEffext(HitResult);
					
				
				}
			
			}
			else if(ADestructibleActor* DA = Cast<ADestructibleActor>(HitResult.GetActor()))
			{

				FVector EyeLocation;
				FRotator EyeRotation;
				MyPawn->GetActorEyesViewPoint(EyeLocation, EyeRotation);

				FVector AttackDirection = EyeRotation.Vector();
				TArray<AActor*> AllActors;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), AXBaseCharacter::StaticClass(), AllActors);
				//I use radial damage on destructible actors to give it an explode effect
				UGameplayStatics::ApplyRadialDamage(GetWorld(), 10.f, HitResult.GetActor()->GetActorLocation(), 30.f, DamageType, AllActors,this, MyPawn->GetInstigatorController(),true, ECC_Weapon);
				
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
		UCombatComponent* CombatComp = Cast<UCombatComponent>(Enemy->GetComponentByClass(UCombatComponent::StaticClass()));

		if (Enemy->GetTeamNumber() != MyPawn->GetTeamNumber() && CombatComp)
		{
			//If we hit a immune enemy just end here
			if (CombatComp->GetBattleState() == EBattleState::PS_Invincible)
			{
				return;
			}
			

			float DealtDamage = Damage * DamageModifier;

			FPointDamageEvent DamageEvent;
			DamageEvent.Damage = DealtDamage;
			DamageEvent.HitInfo = HitResult;
			PlaySound(SwordImpactSounds);
			Enemy->TakeDamage(DealtDamage, DamageEvent, Instigator->GetController(), MyPawn);
			UE_LOG(LogTemp, Warning, TEXT("Damage: %f"), DealtDamage);
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



void AMeleeWeapon::SetDamageModifier(float newDamage) 
{
	DamageModifier = newDamage;
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





