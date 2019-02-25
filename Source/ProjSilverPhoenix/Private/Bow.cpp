// Copyright Project Silver Phoneix

#include "Bow.h"
#include "Projectile.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

ABow::ABow()
{

}

void ABow::StartAttack()
{
	//Spawn the arrow
	SpawnArrow();
	
}

void ABow::StopAttack()
{
}

void ABow::ReleaseAttack()
{
	//release the arrow 
	if (!currentProjectile) { return; }
	currentProjectile->LaunchProjectile(LaunchSpeed);
}

void ABow::SpawnArrow()
{

	

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FVector NewLocation = this->WeaponMesh->GetSocketLocation(ArrowSocket);
	FRotator NewRotaor = this->WeaponMesh->GetSocketRotation(ArrowSocket);


	FHitResult Hit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);

	FVector StartTrace = NewLocation;
	FVector EndTrace = (GetActorForwardVector() * 1000) + StartTrace;

	if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Camera, TraceParams))
	{
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 0, 0), true, 10.f);
	}


	currentProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileToShoot,NewLocation,NewRotaor,SpawnParams);

	currentProjectile->AttachToActor(this,FAttachmentTransformRules::SnapToTargetIncludingScale,ArrowSocket);
	//currentProjectile->AttachToComponent(this->WeaponMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, ArrowSocket);
}
