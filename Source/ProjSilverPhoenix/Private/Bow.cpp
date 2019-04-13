// Copyright Project Silver Phoneix

#include "Bow.h"
#include "Projectile.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "SPlayer.h"

ABow::ABow()
{
	WeaponSocketName = FName("BowSocket");
	BackSocketName = FName("MeleeBackSocket"); //TODO - Create back socket for bow
}

void ABow::BeginPlay()
{
	Super::BeginPlay();
}

void ABow::StartAttack()
{
	//Check weather the player can fire . e.g. ammo, current arrow is not shooting
	if (!GetIsAiming()) return;

	bIsDrawingBow = true;
}

void ABow::ReleaseAttack()
{
	bIsDrawingBow = false;
	//Play fire animation
	
	if (FireBowMontage)
	{
		PlayWeaponAnimation(FireBowMontage, 1.3f);
	}
		//Dont allow player to fire until animation is complete
	SpawnArrow(AimDirection());
}

void ABow::PressFocus()
{
	playerOwner = Cast<ASPlayer>(MyPawn);
	
	if (playerOwner)
	{
		bIsAiming = true;
		playerOwner->SwitchStats(EPlayerStates::PS_Combat);
		playerOwner->AttachWeaponToSocket(this);
		playerOwner->LockPlayerToCameraView(true);
	}
}

void ABow::ReleaseFocus()
{
	bIsAiming = false;
	
	if (playerOwner)
	{
		playerOwner->LockPlayerToCameraView(false);
	}
	//we need to return back to idle
	//check whether we are still drawing the bow
	//if so release it
	//add method to call to set drawing to false
}


void ABow::FireArrow(AProjectile* arrow, FVector arrowVelocity)
{
	currentProjectile->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	currentProjectile->LaunchProjectile(arrowVelocity);
}

void ABow::SpawnArrow(FVector endPoint)
{

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FVector muzzleLoc = this->WeaponMesh->GetSocketLocation(ArrowSpawnSocket);
	FRotator MuzzleRot = this->WeaponMesh->GetSocketRotation(ArrowSpawnSocket);

	FVector arrowVelocity = endPoint - muzzleLoc;
	arrowVelocity.Normalize();
	arrowVelocity *= projectileSpeed;

	if (ProjectileToShoot)
	{
		currentProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileToShoot, WeaponMesh->GetSocketLocation(ArrowSpawnSocket), WeaponMesh->GetSocketRotation(ArrowSpawnSocket), SpawnParams);
		currentProjectile->AttachToComponent(WeaponMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, ArrowSpawnSocket);
		currentProjectile->SetOwner(this);
		FireArrow(currentProjectile, arrowVelocity);
	}

}

FVector ABow::AimDirection()
{
	if (!playerOwner) return FVector(0);

	FHitResult Hit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(MyPawn);

	FVector EyeLocation;
	FRotator EyeRotation;
	playerOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector StartTrace = EyeLocation;
	FVector EndTrace = (EyeRotation.Vector() * 10000) + StartTrace;

	if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Camera, TraceParams))
	{
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 0, 0), true, 10.f);
		return Hit.ImpactPoint;
	}
	
	return EndTrace;
}



bool ABow::GetIsDrawingBow() const
{
	return bIsDrawingBow;
}


