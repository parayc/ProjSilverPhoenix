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

	//SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	//RootComponent = SceneComp;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Comp"));
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//CollisionSphere->SetupAttachment(SceneComp);
	RootComponent = CollisionSphere;
	/*ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMesh->SetupAttachment(CollisionCap);
	ProjectileMesh->bAutoActivate = false;*/
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->UpdatedComponent = CollisionSphere;
	ProjectileMovement->bAutoActivate = false;
	//ProjectileMovement->InitialSpeed = 5000;
	//ProjectileMovement->MaxSpeed = 6000;

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
