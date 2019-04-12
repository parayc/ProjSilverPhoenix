// Copyright Project Silver Phoneix

#include "RangeWeapon.h"
#include "XBaseCharacter.h"
#include "EnemyAIController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

ARangeWeapon::ARangeWeapon()
: 
bIsFiring(false),
bCanFire(true),
bIsReloading(false),
FireRate(0),
CurrentAmmo(0),
MaxAmmoInClip(0),
CurrentAmmoInClip(0)
{
}

FVector ARangeWeapon::GetAdjustedAim()
{
 //This needs to return the players aim direction and the AI aiming direction
	FVector AimDir;
	if (MyPawn)
	{
		AEnemyAIController* AIController = Cast<AEnemyAIController>(MyPawn->GetController());
		if (AIController)
		{
			AimDir = AIController->GetControlRotation().Vector();
			//MyPawn->GetForwardVector();
		}
	}

	return AimDir;
}

void ARangeWeapon::AimTowardsCrossHair()
{
	FHitResult HitLocation;
	auto PawnOwner = Cast<AXBaseCharacter>(GetOwner());
	if (PawnOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		PawnOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector StartLocation;
		FVector TraceEnd = EyeLocation + EyeRotation.Vector() * 1000;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(PawnOwner);
		QueryParams.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(HitLocation, EyeLocation , TraceEnd,ECC_Weapon, QueryParams))
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor(255, 0, 0), true, 10.f);
		}
	}
	

}

int32 ARangeWeapon::GetCurrentAmmoInClip() const
{
	return int32();
}

int32 ARangeWeapon::GetMaxAmmo() const
{
	return int32();
}

int32 ARangeWeapon::GetCurrentAmmo() const
{
	return int32();
}

int32 ARangeWeapon::GetMaxAmmoClip() const
{
	return int32();
}

bool ARangeWeapon::GetIsFiring() const
{
	return bIsFiring;
}

bool ARangeWeapon::SetIsFiring() const
{
	return bIsFiring;
}

bool ARangeWeapon::GetIsAiming() const
{
	return bIsAiming;
}

void  ARangeWeapon::SetIsAiming(bool isAiming) 
{
	bIsAiming = isAiming;
}

void ARangeWeapon::UseAmmo()
{
	CurrentAmmoInClip--;
}
