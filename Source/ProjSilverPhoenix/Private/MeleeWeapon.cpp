// Copyright Project Silver Phoneix

#include "MeleeWeapon.h"
#include "Particles/ParticleSystemComponent.h"
#include "EnemyMaster.h"
#include "ProjSilverPhoenix.h"
#include "XBaseCharacter.h"
#include "DrawDebugHelpers.h"
#include "MeleeAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "DestructibleComponent.h"
#include "CombatComponent.h"
#include "HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/NavMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

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
	bIsAttacking = true;
	bool IsInAir = MyPawn->GetMovementComponent()->IsFalling();
	if (IsInAir)
	{
		AnimInstance->Attack(EAttackType::PS_Air);
	}
	else
	{
		AnimInstance->Attack(EAttackType::PS_Light);
	}
	
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
			DrawDebugLine(Instigator->GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1, 0, 2.0);
		}
		

		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Weapon, TraceParams))
		{
			
			AActor* HitActor = HitResult.GetActor();
			if (!HitActor) return;

			UHealthComponent* HealthComp = HitActor->FindComponentByClass<UHealthComponent>();
			//We check if we hit the enemy already in the same swing and not hit ourself
			if (!EnemiesHit.Contains(HitActor) && HitActor != MyPawn)
			{
	
				if (HealthComp && HealthComp->GetHealth() > 0.0f)
				{
					EnemiesHit.Add(HitActor);
					//Deal damage to enemy that was added
					HandleDamage(HitResult, SwordDamageType);
					SpawnHitEffext(HitResult);
					//We only want to play sounds if the actor got hit or not invinble 
					UCombatComponent* CombatComp = Cast<UCombatComponent>(HitResult.GetActor()->GetComponentByClass(UCombatComponent::StaticClass()));
					
					if (CombatComp && CombatComp->GetBattleState() != EBattleState::PS_Invincible && !UHealthComponent::IsFriendly(MyPawn, HitActor))
					{
						PlaySound(SwordImpactSounds);
						UE_LOG(LogTemp, Warning, TEXT("Play Sound"))
					}
				}
			}
		
		}

	}

	/* Store the location of the previous frame*/
	LastFrameStartSocket = StartSocket;
	LastFrameEndSocket = EndSocket;

}

bool AMeleeWeapon::IsTargetWithinSight(AActor* Target)
{
	if (Target == nullptr) return false;

	FHitResult Hit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(MyPawn);

	FVector StartTrace = WeaponMesh->GetSocketLocation(SocketBase);
	FVector EndTrace = Target->GetActorLocation();
	//Check Whether the enemy is within sight 
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Weapon, TraceParams))
	{
		if (Target == Hit.GetActor())
		{
			return true;
		}

	}

	return false;
}


void AMeleeWeapon::GroundSlamAttack()
{

	TArray<FHitResult> HitResults;
	FVector Start = WeaponMesh->GetSocketLocation(SocketBase);
	FVector End = Start + FVector(0, 0, 50.f);//Line trace a sphere around the player

											 //Setting the collision details
	FCollisionShape CollionShape;
	CollionShape.ShapeType = ECollisionShape::Sphere;
	CollionShape.SetSphere(GroundSlamAttackRadius);

	FCollisionQueryParams CollParam;
	CollParam.AddIgnoredActor(this);//Ignore ourself
	CollParam.AddIgnoredActor(MyPawn);

									//We only look for pawn objects
	//ECollisionChannel ECC = ECollisionChannel::ECC_Pawn;
	if (bDrawDebugLines)
	{
		DrawDebugSphere(GetWorld(), End, GroundSlamAttackRadius, 12, FColor::Yellow, false, 3.f, 0, 1.f);
	}

	//PlaySound
	
	PlaySound(GroundSlamSounds);
	if (GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::FQuat(), ECC_Weapon, CollionShape, CollParam))
	{
		
		for (auto& Hit : HitResults)
		{
			if (Hit.GetActor())
			{

				UHealthComponent* HealthComp = Cast<UHealthComponent>(Hit.GetActor()->GetComponentByClass(UHealthComponent::StaticClass()));
				if (HealthComp && IsTargetWithinSight(Hit.GetActor()) && !EnemiesHit.Contains(Hit.GetActor()))
				{
						EnemiesHit.Add(Hit.GetActor());
						HandleDamage(Hit, GroundSlamDamageType);
						UCombatComponent* CombatComp = Cast<UCombatComponent>(Hit.GetActor()->GetComponentByClass(UCombatComponent::StaticClass()));
						if (CombatComp)
						{
							CombatComp->KnockDown(MyPawn);
						}
					
				}
			}
		
		}
	}
}

void AMeleeWeapon::GroundSlamReset(AActor* HitActor)
{
	if (UCapsuleComponent* PrimitiveComponent = HitActor->FindComponentByClass<UCapsuleComponent>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Primitive"))
			PrimitiveComponent->SetCollisionResponseToChannel(ECC_Weapon, ECollisionResponse::ECR_Block);
	}
}

void AMeleeWeapon::PlaySound(USoundBase * Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation());
	}
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

bool AMeleeWeapon::CanDamage(const FHitResult& HitResult)
{
	//Move this code to the player and ai's take damage method - maybe
	UHealthComponent* HealthComp = Cast<UHealthComponent>(HitResult.GetActor()->GetComponentByClass(UHealthComponent::StaticClass()));
	UCombatComponent* CombatComp = Cast<UCombatComponent>(HitResult.GetActor()->GetComponentByClass(UCombatComponent::StaticClass()));

	if (HealthComp == nullptr) return false;

	//If we hit a immune enemy just end here
	if (CombatComp && CombatComp->GetBattleState() == EBattleState::PS_Invincible)
	{
		return false;
	}

	if (UHealthComponent::IsFriendly(MyPawn, HitResult.GetActor()))
	{
		return false;
	}

	return true;
}

void AMeleeWeapon::HandleDamage(const FHitResult& HitResult, TSubclassOf<UDamageType> DamageType)
{
	if (CanDamage(HitResult))
	{
		DealDamage(HitResult, DamageType);
	}
}

void AMeleeWeapon::DealDamage(const FHitResult& HitResult, TSubclassOf<UDamageType> DamageType)
{
	
	FVector EyeLocation;
	FRotator EyeRotation;
	MyPawn->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector HitPoint = HitResult.ImpactPoint;

	FVector ForwardDir = MyPawn->GetActorForwardVector();
	FVector HitDirection = MyPawn->GetActorLocation() - HitResult.ImpactPoint;

	float DealtDamage = Damage * DamageModifier;
	//We pass the impact point instead of the hit direction
	UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), DealtDamage, HitResult.ImpactPoint, HitResult, MyPawn->GetInstigatorController(), this, DamageType);
	

}



bool AMeleeWeapon::GetIsAttcking() const
{
	return bIsAttacking;
}

bool AMeleeWeapon::GetIsTracing() const
{
	return bIsAttackTrace;
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





