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

void ABow::StartAttack()
{
	if (!GetIsAiming()) return;

	bIsDrawingBow = true;
	//release the arrow 
	if (!currentProjectile) { return; }
	if (FireBowMontage)
	{
		PlayWeaponAnimation(FireBowMontage, 1.3f);
		currentProjectile->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		currentProjectile->LaunchProjectile(LaunchSpeed);
		currentProjectile = nullptr;
	}

	ASPlayer* Owner = Cast<ASPlayer>(MyPawn);
	if (Owner)
		Owner->LockPlayerToCameraView(false);
}

void ABow::ReleaseAttack()
{
	bIsDrawingBow = false;
}

void ABow::PressFocus()
{

	auto PlayerOwner = Cast<ASPlayer>(MyPawn);
	if (PlayerOwner)
	{
		bIsAiming = true;
		PlayerOwner->SwitchStats(EPlayerStates::PS_Combat);
		PlayerOwner->AttachWeaponToSocket(this);
		PlayerOwner->LockPlayerToCameraView(true);
	}
	
	//AimBow();
}

void ABow::ReleaseFocus()
{
	bIsAiming = false;
	ASPlayer* Owner = Cast<ASPlayer>(MyPawn);
	if (Owner)
	{
		Owner->LockPlayerToCameraView(false);
	}
	//we need to return back to idle
}

void ABow::AimBow()
{
	//Getowner and lock the forward diretiont to camera
	ASPlayer* Owner = Cast<ASPlayer>(MyPawn);
	if (Owner)
	{
		Owner->LockPlayerToCameraView(true);
	}
}

void ABow::SpawnArrow()
{

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FVector NewLocation = this->WeaponMesh->GetSocketLocation(ArrowSpawnSocket);
	FRotator NewRotaor = this->WeaponMesh->GetSocketRotation(ArrowSpawnSocket);

	if (ProjectileToShoot)
	{
		currentProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileToShoot, WeaponMesh->GetSocketLocation(ArrowSpawnSocket), WeaponMesh->GetSocketRotation(ArrowSpawnSocket), SpawnParams);
		currentProjectile->AttachToComponent(WeaponMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, ArrowSpawnSocket);
		currentProjectile->SetOwner(this);
	}
	
	

	/*FHitResult Hit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);

	FVector StartTrace = NewLocation;
	FVector EndTrace = (GetActorForwardVector() * 1000) + StartTrace;

	if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Camera, TraceParams))
	{
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 0, 0), true, 10.f);
	}
*/

}

bool ABow::GetIsDrawingBow() const
{
	return bIsDrawingBow;
}
