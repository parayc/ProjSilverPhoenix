// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UProjectileMovementComponent;
class USceneComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArrowFiredSignature);

UCLASS()
class PROJSILVERPHOENIX_API AProjectile : public AActor
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnArrowFiredSignature OnProjectileFired;

public:	
	// Sets default values for this actor's properties
	AProjectile();

	void LaunchProjectile(FVector arrowVelocity);

	void DestroyProjectile(float TimeTakenBeforeDestroyed);

	void SetProjectileDamage(float damage);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	class USceneComponent * SceneComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Projectile")
	class USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Projectile")
	class UStaticMeshComponent* ProjectileMesh;
	
private:
	float Damage;

	bool bCanDestroy = true;

	void DestroyProjectile();

	FTimerHandle DestroyHandle;
	
	
};
