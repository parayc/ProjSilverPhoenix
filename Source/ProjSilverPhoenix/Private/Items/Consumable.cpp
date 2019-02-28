// Copyright Project Silver Phoneix

#include "Consumable.h"



AConsumable::AConsumable()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AConsumable::BeginPlay()
{

}

void AConsumable::UseItem(AActor * OwnerController)
{
	//Call blueprint 
	OnActivited(OwnerController, InventoryRef);

	if (TickInterval <= 0.0f)
	{
		OnTickPowerup();
	}
	else
	{
		GetWorldTimerManager().SetTimer(TimerHandle_ItemTick, this, &AConsumable::OnTickPowerup, TickInterval, true);
	}
}

void AConsumable::OnTickPowerup()
{
	TicksCounter++;

	//Implmenetation is done in blueprints
	/*Allows the designer to make the items do some function every tick*/
	OnItemTick();

	if (TicksCounter >= TotalNumberOfTicks)
	{
		OnExpired();
		//We want to destroy the item once we are finish with it 
		Destroy();
		GetWorldTimerManager().ClearTimer(TimerHandle_ItemTick);
	}
}
