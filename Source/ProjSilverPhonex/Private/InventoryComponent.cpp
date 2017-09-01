// Copyright Project Silver Phoneix

#include "InventoryComponent.h"
#include "BaseItem.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	//Set the size of array
	InventorySlots.SetNum(NumberOfSlots);
}

TArray<FInventorySlots> UInventoryComponent::GetInventorySlots() const
{
	return InventorySlots;
}




void UInventoryComponent::AddItem(TSubclassOf<class ABaseItem> Item, int32 AmountToAdd)
{

	UE_LOG(LogTemp, Warning, TEXT("Add Item: %d"), AmountToAdd)
	if (Item.GetDefaultObject()->ItemInfo.CanBeStacked)
	{

		int32 FoundIndex = SearchFreeStack(Item);
		if (0 <= FoundIndex) //If we find a stack already in inventory
		{
			//Check to see if the current amount and amount to add is greater than max stack size
			if ((InventorySlots[FoundIndex].Amount + AmountToAdd) > MaxStackSize)
			{
				
				int32 NewAmountToAdd = (InventorySlots[FoundIndex].Amount + AmountToAdd) - MaxStackSize;
				InventorySlots[FoundIndex].Amount = MaxStackSize;
				/*Add the remaining amount */
				AddItem(Item, NewAmountToAdd);
				//Update the widget at this index
				OnUpdateRequest.Broadcast(FoundIndex);
			}
			else
			{
				InventorySlots[FoundIndex].Amount += AmountToAdd;
				//Update the widget at this index
				OnUpdateRequest.Broadcast(FoundIndex);
				return;
			}

		}
		else //No Stack found
		{	//Look for empty slot
			FoundIndex = SearchEmptySlot();
			if (0 <= FoundIndex)
			{
				
				InventorySlots[FoundIndex].ItemClass = Item;

				
				if (AmountToAdd > MaxStackSize)
				{
					InventorySlots[FoundIndex].Amount = MaxStackSize;
					/*Check to see if we can add the remaining amount to the inventory*/
					int32 NewAmount = AmountToAdd - MaxStackSize;
					AddItem(Item, NewAmount);

				}
				else
				{
					InventorySlots[FoundIndex].Amount = AmountToAdd;
				}	
				//Update the widget at this index
				OnUpdateRequest.Broadcast(FoundIndex);
				return;
			}
			else
			{
				//No space avaialble
				UE_LOG(LogTemp, Warning, TEXT("No Inventory space"))
			}
		}
	}
	else
	{
		//If we found a empty slot
		int32 FoundIndex = SearchEmptySlot();
		if (0 <= FoundIndex)
		{
			//Add item in the slot index found
			InventorySlots[FoundIndex].ItemClass = Item;
			InventorySlots[FoundIndex].Amount = 1;//1 because item is not stackable


			//Check to see if we still have to add more items
			if (AmountToAdd > 1)
			{
				AddItem(Item, AmountToAdd - 1);
			}

			OnUpdateRequest.Broadcast(FoundIndex);
			return;
		}
		else
		{
			//No space to add item
			UE_LOG(LogTemp, Warning, TEXT("No Inventory space"))
		}
		
	}
	
}

bool UInventoryComponent::IsSlotEmpty(int32 Index)
{
	if (InventorySlots[Index].ItemClass == nullptr)
	{
		return true;

	}

	return false;
}

int32 UInventoryComponent::SearchEmptySlot()
{
	int32 SlotFound = -1;

	for (int i = 0; i < InventorySlots.Num(); i++)
	{
		if (InventorySlots[i].ItemClass == nullptr)
		{
			SlotFound = i;
			return SlotFound;
		}
	}

	return SlotFound;
}

int32 UInventoryComponent::SearchFreeStack(TSubclassOf<class ABaseItem> Item)
{
	int32 Index = -1;

	for (int i = 0; i < InventorySlots.Num(); i++)
	{
		if (InventorySlots[i].ItemClass == Item && InventorySlots[i].Amount < MaxStackSize)
		{
			UE_LOG(LogTemp, Warning, TEXT("Stack Space found"))
			Index = i;
			return Index;
		}
	}  

	UE_LOG(LogTemp, Warning, TEXT("No Stack Space found"))
	return Index;
	

}

int32 UInventoryComponent::GetAmountAtIndex(int32 index) const
{
	return InventorySlots[index].Amount;
}

FItemInfo UInventoryComponent::GetItemInfoAtIndex(int32 index) const
{
	return InventorySlots[index].ItemClass.GetDefaultObject()->ItemInfo;
}



// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

