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
#include "HealthComponent.h"

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
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(MyPawn);
	TraceParams.AddIgnoredActor(this);
	TraceParams.bTraceComplex = true;

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
		

		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Weapon, TraceParams))
		{
			AActor* HitActor = HitResult.GetActor();
			//We check if we hit the enemy already in the same swing and not hit ourself
			if (HitActor && !EnemiesHit.Contains(HitActor) && HitActor != MyPawn)
			{
				EnemiesHit.Add(HitActor);
				UHealthComponent* HealthComp = HitActor->FindComponentByClass<UHealthComponent>();
				if (HealthComp && HealthComp->GetHealth() > 0.0f)
				{
					//Deal damage to enemy that was added
					DealDamage(HitResult);
					SpawnHitEffext(HitResult);
				}
				
				

			}else if (ADestructibleActor* DA = Cast<ADestructibleActor>(HitResult.GetActor()))
			{

				FVector EyeLocation;
				FRotator EyeRotation;
				MyPawn->GetActorEyesViewPoint(EyeLocation, EyeRotation);

				FVector AttackDirection = EyeRotation.Vector();
				TArray<AActor*> AllActors;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), AXBaseCharacter::StaticClass(), AllActors);
				//I use radial damage on destructible actors to give it an explode effect
				UGameplayStatics::ApplyRadialDamage(GetWorld(), 10.f, HitResult.GetActor()->GetActorLocation(), 30.f, DamageType, AllActors, this, MyPawn->GetInstigatorController(), true, ECC_Weapon);

			}
			
		
		}
		//ELSE if ...


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
	
	UHealthComponent* HealthComp = Cast<UHealthComponent>(HitResult.GetActor()->GetComponentByClass(UHealthComponent::StaticClass()));
	UCombatComponent* CombatComp = Cast<UCombatComponent>(HitResult.GetActor()->GetComponentByClass(UCombatComponent::StaticClass()));

	//If we hit a immune enemy just end here
	if (CombatComp && CombatComp->GetBattleState() == EBattleState::PS_Invincible)
	{
		return;
	}
	
	if (HealthComp)
	{

		if (UHealthComponent::IsFriendly(MyPawn, HitResult.GetActor()))
		{
			return;
		}

			float DealtDamage = Damage * DamageModifier;
			FPointDamageEvent DamageEvent;
			DamageEvent.Damage = DealtDamage;
			DamageEvent.HitInfo = HitResult;
			TSubclassOf<UDamageType> p;
			PlaySound(SwordImpactSounds);
			UGameplayStatics::ApplyDamage(HitResult.GetActor(), DealtDamage, MyPawn->GetInstigatorController(), this, p);

	
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





