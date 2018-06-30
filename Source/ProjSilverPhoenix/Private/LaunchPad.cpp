// Copyright Project Silver Phoneix

#include "LaunchPad.h"
//#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "XBaseCharacter.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"

// Sets default values
ALaunchPad::ALaunchPad()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;
	SphereComp->SetSphereRadius(30.f);

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ALaunchPad::HanleOverlapped);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(SphereComp);

	LaunchStrength = 1500;
	LaunchPitAngle = 35.f;

}

// Called when the game starts or when spawned
void ALaunchPad::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALaunchPad::HanleOverlapped(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	
	FRotator LaunchDirection = GetActorRotation();
	LaunchDirection.Pitch += LaunchPitAngle;
	FVector LaunchVelocity = LaunchDirection.Vector() * LaunchStrength;


	auto BaseCharacter = Cast<ACharacter>(OtherActor);
	if (BaseCharacter)
	{
		UGameplayStatics::SpawnSoundAttached(LaunchPadSound, SphereComp);
		BaseCharacter->LaunchCharacter(LaunchVelocity,true,true);
		UGameplayStatics::SpawnEmitterAtLocation(this, LaunchPadEffect,GetActorLocation());
	
	}
}

