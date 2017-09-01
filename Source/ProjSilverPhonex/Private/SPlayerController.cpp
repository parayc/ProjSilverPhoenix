// Copyright Project Silver Phoneix

#include "SPlayerController.h"
#include "InventoryComponent.h"
#include "BaseItem.h"


void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayersInventory = FindComponentByClass<UInventoryComponent>();
	if (!ensure(PlayersInventory)) { 
		UE_LOG(LogTemp, Error, TEXT("No Inventory Component found on player controller"));
		return;
	
	}
}

void ASPlayerController::AddItemToInventory(TSubclassOf<class ABaseItem> ItemClass, int32 Amount)
{
	if (ItemClass)
	{
		PlayersInventory->AddItem(ItemClass, Amount);
	}
	
}