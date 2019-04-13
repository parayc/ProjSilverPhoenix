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
	//Check weather the player can fire . e.g. ammo, current arrow is not shooting, are they aiming
	if (!CanFire()) return;

	bIsDrawingBow = true;
}

void ABow::ReleaseAttack()
{
	bIsDrawingBow = false;

	if (!CanFire()) return;

	if (FireBowMontage)
	{
		PlayWeaponAnimation(FireBowMontage, 1.3f);
	}
		//Dont allow player to fire until animation is complete
	SpawnArrow(AimDirection());
}

void ABow::PressFocus()
{
	ASPlayer* playerOwner = Cast<ASPlayer>(MyPawn);
	
	if (playerOwner)
	{
		bIsAiming = true;
		Zoom(bIsAiming);
		playerOwner->SwitchStats(EPlayerStates::PS_Combat);
		playerOwner->AttachWeaponToSocket(this);
		playerOwner->LockPlayerToCameraView(true);
	}
}

void ABow::ReleaseFocus()
{
	bIsAiming = false;
	Zoom(bIsAiming);
	ASPlayer* playerOwner = Cast<ASPlayer>(MyPawn);
	if (playerOwner)
	{
		playerOwner->LockPlayerToCameraView(false);
	}

	//stop the player from drawing the bow when they are not aiming
	bIsDrawingBow = false;
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
	ASPlayer* playerOwner = Cast<ASPlayer>(MyPawn);
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
	DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 0, 0), true, 10.f);
	return EndTrace;
}

bool ABow::CanFire()
{
	return GetIsAiming();
}

void ABow::Zoom(bool bZooming)
{
	ASPlayer* playerOwner = Cast<ASPlayer>(MyPawn);
	if (playerOwner)
	{
		playerOwner->ZoomCamera(bZooming, ZoomFOV);
	}
}

bool ABow::GetIsDrawingBow() const
{
	return bIsDrawingBow;
}


