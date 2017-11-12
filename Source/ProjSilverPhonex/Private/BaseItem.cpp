// Copyright Project Silver Phoneix

#include "BaseItem.h"
#include "InventoryComponent.h"
#include "Components/SphereComponent.h"


// Sets default values
ABaseItem::ABaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = ItemStaticMesh;

	PickUpSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickUpSphere"));
	PickUpSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}


void ABaseItem::UseItem()
{
	
	UE_LOG(LogTemp, Warning, TEXT("Used Item"))
	InventoryRef->RemoveItemFromIndex(Index, 1);
	OnUseItemRequest.Broadcast();
	Destroy();//TODO - update to support items we dont want to destroy once we used it
}


// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

