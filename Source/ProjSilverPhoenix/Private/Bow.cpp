// Copyright Project Silver Phoneix

#include "Bow.h"
#include "Projectile.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

ABow::ABow()
{
	WeaponSocketName = FName("BowSocket");
	BackSocketName = FName("MeleeBackSocket"); //TODO - Create back socket for bow
}

void ABow::StartAttack()
{
	//Play the animation
	//Spawn the arrow
	if (DrawBowAnim)
	{
		PlayWeaponAnimation(DrawBowAnim, 1.3f);
	}

	//TODO - Needs to be called in animation
	SpawnArrow();
	
}

void ABow::StopAttack()
{
}

void ABow::ReleaseAttack()
{
	//release the arrow 

	if (!currentProjectile) { return; }
	if (FireBowAnim)
	{
		bIsFiring = true;
		PlayWeaponAnimation(FireBowAnim,1.3f);
		currentProjectile->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		currentProjectile->LaunchProjectile(LaunchSpeed);
		currentProjectile = nullptr;
	}
	

}

void ABow::SpawnArrow()
{

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FVector NewLocation = this->WeaponMesh->GetSocketLocation(ArrowSocket);
	FRotator NewRotaor = this->WeaponMesh->GetSocketRotation(ArrowSocket);

	if (ProjectileToShoot)
	{
		currentProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileToShoot, WeaponMesh->GetSocketLocation(ArrowSocket), WeaponMesh->GetSocketRotation(ArrowSocket), SpawnParams);
		currentProjectile->AttachToComponent(WeaponMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, ArrowSocket);
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
