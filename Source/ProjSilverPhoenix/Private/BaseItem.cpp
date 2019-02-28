// Copyright Project Silver Phoneix

#include "BaseItem.h"
#include "InventoryComponent.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"


// Sets default values
ABaseItem::ABaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}



// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	
}

/* This will call the item BP implmenentation */
void ABaseItem::UseItem(AActor* OwnerController)
{

	//InventoryRef->RemoveItemFromIndex(Index, 1);
	//OnUseItemRequest.Broadcast();

	//Call blueprint 
	//OnActivited(OwnerController, InventoryRef);

	//if (TickInterval <= 0.0f)
	//{
	//	OnTickPowerup();
	//}
	//else
	//{
	//	GetWorldTimerManager().SetTimer(TimerHandle_ItemTick, this, &ABaseItem::OnTickPowerup, TickInterval, true);
	//}
}



//void ABaseItem::OnTickPowerup()
//{
//	//TicksCounter++;
//
//	////Implmenetation is done in blueprints
//	///*Allows the designer to make the items do some function every tick*/
//	//OnItemTick();
//
//	//if (TicksCounter >= TotalNumberOfTicks)
//	//{
//	//	OnExpired();
//	//	GetWorldTimerManager().ClearTimer(TimerHandle_ItemTick);
//	//}
//}

// Called every frame
void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
}

