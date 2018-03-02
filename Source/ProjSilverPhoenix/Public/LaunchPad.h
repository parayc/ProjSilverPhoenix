// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaunchPad.generated.h"
class UBoxComponent;
class USoundCue;

UCLASS()
class PROJSILVERPHOENIX_API ALaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaunchPad();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* BoxComp;
	UPROPERTY(EditDefaultsOnly, Category = "FX")
		UParticleSystem* LaunchPadEffect;

	UPROPERTY(EditInstanceOnly, Category = "LaunchPad")
		int32 LaunchStrength = 1000;
	UPROPERTY(EditInstanceOnly, Category = "LaunchPad")
		float LaunchPitAngle = 35.f;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		USoundCue* LaunchPadSound;

	UFUNCTION()
	void HanleOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
public:	


	
	
};
