// Copyright Project Silver Phoneix

#include "SPlayerController.h"
#include "InventoryComponent.h"
#include "BaseItem.h"
#include "SPlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"


void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerCameraManagerClass = ASPlayerCameraManager::StaticClass();

	PlayersInventory = FindComponentByClass<UInventoryComponent>();
	if (!ensure(PlayersInventory)) { 
		UE_LOG(LogTemp, Error, TEXT("No Inventory Component found on player controller"));
		return;
	
	}

}

