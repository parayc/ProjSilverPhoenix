// Copyright Project Silver Phoneix


#include "Projectile.h"
#include "Components/CapsuleComponent.h"	
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	RootComponent = SceneComp;

	CollisionCap = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision Comp"));
	CollisionCap->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionCap->SetupAttachment(SceneComp);

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
	// GetActorForwardVector()
	FHitResult HitLocation;
	ProjectileMovement->Activate();
	FVector StartLocation = GetActorLocation();
	FVector TraceEnd = StartLocation + GetActorForwardVector() * speed;
	FCollisionQueryParams QueryParams;
	//QueryParams.AddIgnoredActor(PawnOwner);
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitLocation, StartLocation, TraceEnd, ECC_Camera, QueryParams))
	{
		DrawDebugLine(GetWorld(), StartLocation, TraceEnd, FColor(255, 0, 0), true, 10.f);
	}
	ProjectileMovement->SetVelocityInLocalSpace(TraceEnd);

}
