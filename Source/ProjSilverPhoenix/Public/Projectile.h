// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"


class UStaticMeshComponent;
class USphereComponent;
class UProjectileMovementComponent;
class USceneComponent;
UCLASS()
class PROJSILVERPHOENIX_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	void LaunchProjectile(FVector arrowVelocity);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	class UProjectileMovementComponent* ProjectileMovement;

	class USceneComponent * SceneComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Projectile")
	class USphereComponent* CollisionSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Projectile")
	class UStaticMeshComponent* ProjectileMesh;
	

	
	
};
