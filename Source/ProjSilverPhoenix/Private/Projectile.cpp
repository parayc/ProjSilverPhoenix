// Copyright Project Silver Phoneix


#include "Projectile.h"
#include "Components/CapsuleComponent.h"	
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebugHelpers.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionCap = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision Comp"));
	RootComponent = CollisionCap;
	CollisionCap->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/*ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMesh->SetupAttachment(CollisionCap);
	ProjectileMesh->bAutoActivate = false;*/
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->UpdatedComponent = CollisionCap;
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


void AProjectile::LaunchProjectile(float speed)
{
	

	//CollisionCap->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileMovement->Activate();
	ProjectileMovement->SetVelocityInLocalSpace(GetActorForwardVector() * speed);

}
