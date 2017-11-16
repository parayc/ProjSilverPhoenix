// Copyright Project Silver Phoneix

#include "RangeWeapon.h"
#include "XBaseCharacter.h"
#include "EnemyAIController.h"


void ARangeWeapon::StartAttack()
{
	
	if (MyPawn && MyPawn->GetIsDead())
	{
		if (CurrentAmmoInClip > 0 && bCanFire)
		{
			bIsFiring = true;
			StartFire();

			float TimeDelay = FireRate > 0 ? 1 / (FireRate*0.01667) : FApp::GetDeltaTime();

			if (!FireRateHandle.IsValid())
			{

				GetWorld()->GetTimerManager().SetTimer(FireRateHandle, this, &ARangeWeapon::StartAttack, TimeDelay, true);//This will loop the startfire because we set it to true

			}
		}
	}
}
void ARangeWeapon::StopAttack()
{
}

void ARangeWeapon::StartFire()
{
	//Use ammo
	UseAmmo();
}

void ARangeWeapon::StopFire()
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

void ARangeWeapon::UseAmmo()
{
	CurrentAmmoInClip--;
}
