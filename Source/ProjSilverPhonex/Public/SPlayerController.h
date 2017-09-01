// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

/**
 * 
 */
class UInventoryComponent;
class ABaseItem;

UCLASS()
class PROJSILVERPHONEX_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()


	UInventoryComponent* PlayersInventory;
	
public:

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItemToInventory(TSubclassOf<class ABaseItem> ItemClass,int32 Amount);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
