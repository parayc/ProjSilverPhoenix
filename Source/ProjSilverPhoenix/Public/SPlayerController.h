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
class PROJSILVERPHOENIX_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UInventoryComponent* PlayersInventory;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
