// Copyright Project Silver Phoneix

#include "Bow.h"
#include "Projectile.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "SPlayer.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"

ABow::ABow()
{
	WeaponSocketName = FName("BowSocket");
	BackSocketName = FName("MeleeBackSocket"); //TODO - Create back socket for bow

	LaunchSpeed = 0;
	CameraOffset = FVector(0, 40, 0);
}

void ABow::BowCharging()
{
	LaunchSpeed = InitialLaunchSpeed;
	GetWorldTimerManager().SetTimer(BowDrawingTimeHandle,this, &ABow::CalculateProjectileSpeed, BowDrawingChargeRate,true,0.0f);
}

void ABow::BowFullyCharged()
{
	GetWorldTimerManager().ClearTimer(BowDrawingTimeHandle);
}

void ABow::CalculateProjectileSpeed()
{
	LaunchSpeed += LaunchSpeed * PercentageIncrease;
	UE_LOG(LogTemp, Warning, TEXT("Launch: %f"), LaunchSpeed);
}

void ABow::StartAttack()
{
	//Check weather the player can fire . e.g. ammo, current arrow is not shooting, are they aiming
	if (!CanFire()) return;

	OnBowCharged.Broadcast();
	bIsDrawingBow = true;
}

void ABow::ReleaseAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("RA Launch: %f"), LaunchSpeed);
	bIsDrawingBow = false;
	GetWorldTimerManager().ClearTimer(BowDrawingTimeHandle);

	if (!CanFire()) return;

	OnDrawBowEnd.Broadcast();

	if (FireBowMontage)
	{
		PlayWeaponAnimation(FireBowMontage, 1.0f);
	}

	//TODO - Dont allow player to fire until animation is complete
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
	OnDrawBowEnd.Broadcast();//Update UI if we cancel the bow charging

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
	currentProjectile->DestroyProjectile(5.f);
	currentProjectile = nullptr;
}

void ABow::SpawnArrow(FVector endPoint)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FVector muzzleLoc = this->WeaponMesh->GetSocketLocation(ArrowSpawnSocket);
	FRotator MuzzleRot = this->WeaponMesh->GetSocketRotation(ArrowSpawnSocket);

	FVector arrowVelocity = endPoint - muzzleLoc;
	arrowVelocity.Normalize();
	
	UE_LOG(LogTemp, Warning, TEXT("SA LunchSpeed: %f"), LaunchSpeed);
	float projectileSpeed = LaunchSpeed * LaunchSpeed;
	
	projectileSpeed = FMath::Clamp(projectileSpeed, minProjectileSpeed, maxProjectileSpeed);
	arrowVelocity *= projectileSpeed;

	UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), projectileSpeed);

	if (ProjectileToShoot)
	{
		currentProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileToShoot, WeaponMesh->GetSocketLocation(ArrowSpawnSocket), WeaponMesh->GetSocketRotation(ArrowSpawnSocket), SpawnParams);
		currentProjectile->AttachToComponent(WeaponMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, ArrowSpawnSocket);
		currentProjectile->SetOwner(this);
		FireArrow(currentProjectile, arrowVelocity);
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("Error - No projectile selected in %s"),*this->GetName());

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
		//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 0, 0), true, 10.f);
		return Hit.ImpactPoint;
	}
	//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 0, 0), true, 10.f);
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
		//This adds a offset and doesnt ovewrite it
		playerOwner->ZoomCamera(bZooming, CameraOffset, ZoomFOV);
	}
}

bool ABow::GetIsDrawingBow() const
{
	return bIsDrawingBow;
}


