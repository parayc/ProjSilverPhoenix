// Copyright Project Silver Phoneix


#include "Projectile.h"
#include "Components/SphereComponent.h"	
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Comp"));
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	RootComponent = CollisionSphere;

	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->UpdatedComponent = CollisionSphere;
	ProjectileMovement->bAutoActivate = false;


}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AProjectile::LaunchProjectile(FVector arrowVelocity)
{

	//CollisionCap->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	ProjectileMovement->Activate();
	ProjectileMovement->Velocity = arrowVelocity;
	//ProjectileMovement->SetVelocityInLocalSpace(arrowVelocity);

}

void AProjectile::DestroyProjectile(float TimeTakenBeforeDestroyed)
{
	if (!bCanDestroy) return;

	bCanDestroy = false;
	GetWorldTimerManager().SetTimer(DestroyHandle, this, &AProjectile::DestroyProjectile, TimeTakenBeforeDestroyed,false);
}

void AProjectile::DestroyProjectile()
{
	GetWorldTimerManager().ClearTimer(DestroyHandle);
	Destroy();
}
