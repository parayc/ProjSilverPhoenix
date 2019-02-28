// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Consumable.generated.h"

/**
 * 
 */
UCLASS()
class PROJSILVERPHOENIX_API AConsumable : public ABaseItem
{
	GENERATED_BODY()
	
public:

	AConsumable();

	//This is called 
	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
		void OnActivited(AActor* OwnerController, UInventoryComponent* InventoryCompRef);

	//This is called 
	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
		void OnExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
		void OnItemTick();

	 void UseItem(AActor* OwnerController) override;
	  

protected:
	// Called when the game starts or when spawned
	 void BeginPlay() override;

	UFUNCTION()
		void OnTickPowerup();

	/*Time between ticks - if Tick interval is zero, item wont tick*/
	UPROPERTY(EditDefaultsOnly, category = "Powerurps")
		float TickInterval;

	/*The total amount of time the item will tick once used */
	UPROPERTY(EditDefaultsOnly, category = "Powerurps")
		int32 TotalNumberOfTicks;

	FTimerHandle TimerHandle_ItemTick;

	/*Total number of ticks applied*/
	int32 TicksCounter;
	
};
