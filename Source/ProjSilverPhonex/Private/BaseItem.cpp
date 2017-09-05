// Copyright Project Silver Phoneix

#include "BaseItem.h"
#include "InventoryComponent.h"


// Sets default values
ABaseItem::ABaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseItem::OnUseItem()
{
	UE_LOG(LogTemp, Warning, TEXT("Item Used"))
	InventoryRef->RemoveItemFromIndex(Index, 1);
	Destroy();
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

