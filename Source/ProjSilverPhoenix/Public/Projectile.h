// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"


class UStaticMeshComponent;
class UCapsuleComponet;
class UProjectileMovementComponent;
class USceneComponent;
UCLASS()
class PROJSILVERPHOENIX_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	void LaunchProjectile(float speed);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	class USceneComponent * SceneComp;
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	class UCapsuleComponent* CollisionCap;
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	class UStaticMeshComponent* ProjectileMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	class UProjectileMovementComponent* ProjectileMovement;

	
	
};
