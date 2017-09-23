// Copyright Project Silver Phoneix

#include "BattleCircle.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"

// Sets default values for this component's properties
UBattleCircle::UBattleCircle()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	BattleCircle = CreateDefaultSubobject<USphereComponent>(TEXT("BattleCircle"));

	// ...
}


// Called when the game starts
void UBattleCircle::BeginPlay()
{
	Super::BeginPlay();

	// ...
	auto name = GetOwner()->GetName();
	auto root = GetOwner()->GetRootComponent()->GetName();
	
	//UE_LOG(LogTemp, Warning, TEXT("hI %s"), *rooot)

}


// Called every frame
void UBattleCircle::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

