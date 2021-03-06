// Copyright Project Silver Phoneix


#include "Projectile.h"
#include "Components/SphereComponent.h"	
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Comp"));
	RootComponent = SceneComp;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	//ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMesh->SetupAttachment(RootComponent);

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Comp"));
	//CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionSphere->AttachToComponent(ProjectileMesh,FAttachmentTransformRules::KeepRelativeTransform);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->UpdatedComponent = RootComponent;
	ProjectileMovement->bAutoActivate = false;

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (CollisionSphere)
	{
		CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnHit);
	}
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OtherActor %s"), *OtherActor->GetName());
	if (OtherActor == nullptr) return;
	
	UHealthComponent* HealthComp = OtherActor->FindComponentByClass<UHealthComponent>();
	if (HealthComp)
	{
		FVector HitDirection = OtherActor->GetActorLocation() - GetActorLocation();
		if (GetOwner())
		{
			APawn* playerPawnOwner = Cast<APawn>(GetOwner()->GetOwner());
			AController* pawnController = playerPawnOwner->GetController();
			if (pawnController)
			{
				UGameplayStatics::ApplyPointDamage(SweepResult.GetActor(), Damage, OtherActor->GetActorLocation(), SweepResult, pawnController, this, ProjectileDamageType);
				//UE_LOG(LogTemp, Error, TEXT("ActorHit %s"), *SweepResult.GetActor()->GetName());
				//UE_LOG(LogTemp, Warning, TEXT("Impact %s"), *Hit.ImpactPoint.ToString());
			}
			
		}
	}
	Destroy();
}

void AProjectile::SetDamageProperties(float damage, TSubclassOf<UDamageType> DamageType)
{
	Damage = damage;
	ProjectileDamageType = DamageType;
}

void AProjectile::LaunchProjectile(FVector arrowVelocity)
{
	OnProjectileFired.Broadcast();
	//CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProjectileMovement->Activate();
	ProjectileMovement->Velocity = arrowVelocity;
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
