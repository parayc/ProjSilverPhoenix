// Copyright Project Silver Phoneix

#include "Bow.h"
#include "Projectile.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "SPlayer.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "GameFramework/MovementComponent.h"
#include "ProjSilverPhoenix.h"


ABow::ABow()
{
	WeaponSocketName = FName("BowSocket");
	BackSocketName = FName("MeleeBackSocket"); //TODO - Create back socket for bow

	LaunchSpeed = 0;
	CameraOffset = FVector(0, 5, 0);
}

void ABow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABow::BowCharging()
{
	LaunchSpeed = InitialLaunchSpeed;
	CurrentDamage = BaseDamage;
	GetWorldTimerManager().SetTimer(BowDrawingTimeHandle,this, &ABow::CalculateProjectileSpeed, BowDrawingChargeRate,true,0.0f);
	GetWorldTimerManager().SetTimer(BowDamageTimeHandle, this, &ABow::CalculateProjectileDamage, BowDrawingChargeRate, true, 0.0f);
}

void ABow::BowFullyCharged()
{
	GetWorldTimerManager().ClearTimer(BowDrawingTimeHandle);
	GetWorldTimerManager().ClearTimer(BowDamageTimeHandle);
}

void ABow::CalculateProjectileSpeed()
{
	LaunchSpeed += LaunchSpeed * PercentageIncrease;
}

void ABow::CalculateProjectileDamage()
{
	CurrentDamage += 6.f; //TODO - remove magic number
}

void ABow::StartAttack()
{
	//Check weather the player can fire . e.g. ammo, current arrow is not shooting, are they aiming
	if (!GetIsAiming()) return;
	OnBowCharged.Broadcast();
	bIsDrawingBow = true;
	SpawnArrow();
}

void ABow::ReleaseAttack()
{
	bIsDrawingBow = false;
	GetWorldTimerManager().ClearTimer(BowDrawingTimeHandle);

	if (!CanFire()) return;

	OnDrawBowEnd.Broadcast();

	if (FireBowMontage)
	{
		bIsFiring = true;
		float duration = PlayWeaponAnimation(FireBowMontage, 1.f);
		GetWorldTimerManager().SetTimer(OnFireEndTimeHandle, this, &ABow::OnFireEnd, duration, true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error - No Bow Firing animation selected for %s"), *this->GetName());
	}

	//UE_LOG(LogTemp, Error, TEXT("Damage %f"), CurrentDamage);
	auto arrorVelcity = CalculateArrowVelocity(AimDirection());
	float damage = CurrentDamage;
	FireArrow(arrorVelcity, damage);
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

void ABow::FireArrow(FVector arrowVelocity, float arrowDamage)
{
	if (!currentProjectile) return;

	currentProjectile->SetProjectileDamage(arrowDamage);
	currentProjectile->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	currentProjectile->LaunchProjectile(arrowVelocity);
	currentProjectile->DestroyProjectile(5.f);
	currentProjectile = nullptr;
}

void ABow::OnFireEnd()
{
	bIsFiring = false;
	GetWorldTimerManager().ClearTimer(OnFireEndTimeHandle);
}

void ABow::SpawnArrow()
{

	if (!ProjectileToShoot)
	{
		UE_LOG(LogTemp, Error, TEXT("Error - No projectile selected in %s"), *this->GetName());
		return;
	}

	//Don't spawn arrow if we already have arrow 
	if (currentProjectile) return;
		
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector arrowSocketLoc = WeaponMesh->GetSocketLocation(ArrowRestSocket);
	FRotator arrowSocketRot = WeaponMesh->GetSocketRotation(ArrowRestSocket);
	
	currentProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileToShoot, arrowSocketLoc, arrowSocketRot, SpawnParams);
	currentProjectile->AttachToComponent(WeaponMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, ArrowRestSocket);
	currentProjectile->SetOwner(this);

}

FVector ABow::CalculateArrowVelocity(FVector endPoint)
{

	FVector muzzleLoc = WeaponMesh->GetSocketLocation(ArrowRestSocket);
	FVector arrowVelocity = endPoint - muzzleLoc;
	arrowVelocity.Normalize();

	float projectileSpeed = LaunchSpeed * LaunchSpeed;

	projectileSpeed = FMath::Clamp(projectileSpeed, minProjectileSpeed, maxProjectileSpeed);
	arrowVelocity *= projectileSpeed;

	return arrowVelocity;
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
	FVector EndTrace = (EyeRotation.Vector() * 5500) + StartTrace;

	FVector EndPoint = EndTrace;
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_RangeWeapon, TraceParams))
	{
		//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 0, 0), true, 10.f);
		EndPoint = Hit.ImpactPoint;
	}
	//DrawDebugLine(GetWorld(), StartTrace, EndPoint, FColor(255, 0, 0), true, 10.f);
	
	FVector weapon = WeaponMesh->GetSocketLocation(ArrowRestSocket);
	if (GetWorld()->LineTraceSingleByChannel(Hit, weapon, EndPoint, ECC_RangeWeapon, TraceParams))
	{
		EndPoint = Hit.ImpactPoint;
	}
	//DrawDebugLine(GetWorld(), weapon, EndPoint, FColor(255, 255, 0), true, 10.f);
	
	return EndPoint;
}

bool ABow::CanFire()
{
	return GetIsAiming() && !bIsFiring;
}

void ABow::Zoom(bool bZooming)
{
	ASPlayer* playerOwner = Cast<ASPlayer>(MyPawn);
	if (playerOwner)
	{
		FVector TargetOffset = playerOwner->GetDefaultSpringArmSocket() + CameraOffset;
		playerOwner->ZoomCamera(bZooming, TargetOffset, ZoomFOV);
	}
}

bool ABow::GetIsDrawingBow() const
{
	return bIsDrawingBow;
}

bool ABow::CanAttachToBowString() const
{
	if (!MyPawn) return false;

	ASPlayer* playerOwner = Cast<ASPlayer>(MyPawn);
	if (playerOwner)
	{
		return !GetIsFiring() && !playerOwner->GetIsJumping() && (GetIsDrawingBow() || GetIsAiming() || MyPawn->GetCurrentState() == EPlayerStates::PS_Combat);
	}
	
	return false;
}


