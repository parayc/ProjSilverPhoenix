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

int32 UInventoryComponent::GetNumberOfSlots() const
{
	return NumberOfSlots;
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

void UInventoryComponent::RemoveItemFromIndex(int32 Index, int32 AmountToRemove)
{
	//Check to see if the slot is not empty
	if (IsSlotEmpty(Index) != true && AmountToRemove > 0)
	{
		//If the AmounToRemove is greater than the current amount item from slot
		if (AmountToRemove >= InventorySlots[Index].Amount)
		{
			InventorySlots[Index].Amount = 0;
			InventorySlots[Index].ItemClass = NULL;
			OnUpdateRequest.Broadcast(Index);

		}
		else
		{
			//Decrease the current amount in slot
			InventorySlots[Index].Amount = InventorySlots[Index].Amount - AmountToRemove;
			OnUpdateRequest.Broadcast(Index);
		}

		//TODO - Check whether to spawn item or delete it when removing ti from inventory 
		
		for (int i = 0; i < AmountToRemove; i++)
		{
			//FActorSpawnParameters SpawnParams;
			//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			//ABaseItem* ItemSpawned = GetWorld()->SpawnActor<ABaseItem>(InventorySlots[Index].ItemClass, FVector(0, 0, -500), FRotator(0, 0, -500), SpawnParams);
		}
	}
}

void UInventoryComponent::SwapSlots(int32 Index1, int32 Index2)
{
	//Check if its in range of the array fisrt
	if (Index1 > InventorySlots.Num() || Index2 > InventorySlots.Num())
	{
		return;
	}
	else
	{
		//Swaps the elements in array
		auto TempVar = InventorySlots[Index1];
		InventorySlots[Index1] = InventorySlots[Index2];
		InventorySlots[Index2] = TempVar;

		//Update Slots
		OnUpdateRequest.Broadcast(Index1);
		OnUpdateRequest.Broadcast(Index2);
	}
}

void UInventoryComponent::SplitStacks(int32 Index, int32 Amount)
{
	if (!IsSlotEmpty(Index))
	{
		if (GetItemInfoAtIndex(Index).CanBeStacked && GetAmountAtIndex(Index) > Amount)
		{
			int32 FoundIndex = SearchEmptySlot();
			if (-1 < FoundIndex)
			{
				
				//Remove the requested amount from index
				InventorySlots[Index].Amount = InventorySlots[Index].Amount - Amount;

				//And added it to the empty slot found
				InventorySlots[FoundIndex].ItemClass = InventorySlots[Index].ItemClass;
				InventorySlots[FoundIndex].Amount = Amount;

				OnUpdateRequest.Broadcast(Index);
				OnUpdateRequest.Broadcast(FoundIndex);
				return;
			}
			else
			{
				return;
			}
		}
		
	}
}

void UInventoryComponent::UseItemAtIndex(int32 Index)
{
	if (InventorySlots[Index].ItemClass != nullptr)
	{
		//GetWorld()->SpawnActor<ABaseItem>()
		//Would crash if the player walks over the item that they spawned to use

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		//Spawn the item under the level
		ABaseItem* ItemSpawned = GetWorld()->SpawnActor<ABaseItem>(InventorySlots[Index].ItemClass, FVector(0, 0, -500), FRotator(0, 0, -500), SpawnParams);
		ItemSpawned->InventoryRef = this;
		ItemSpawned->Index = Index;
	
		RemoveItemFromIndex(Index, 1);
		ItemSpawned->UseItem(GetOwner());
		
	}
}

bool UInventoryComponent::AddToIndex(int32 FromIndex, int32 ToIndex)
{
	if (InventorySlots[FromIndex].ItemClass == InventorySlots[ToIndex].ItemClass && InventorySlots[ToIndex].Amount < MaxStackSize && GetItemInfoAtIndex(FromIndex).CanBeStacked)
	{
		UE_LOG(LogTemp, Warning, TEXT("Add to index"))
		if (MaxStackSize - GetAmountAtIndex(ToIndex) >= GetAmountAtIndex(FromIndex))
		{
			InventorySlots[ToIndex].Amount = GetAmountAtIndex(FromIndex) + GetAmountAtIndex(ToIndex);
			InventorySlots[ToIndex].ItemClass = InventorySlots[FromIndex].ItemClass;

			InventorySlots[FromIndex].ItemClass = nullptr;
			InventorySlots[FromIndex].Amount = 0;

			OnUpdateRequest.Broadcast(FromIndex);
			OnUpdateRequest.Broadcast(ToIndex);

			
			return true;
		}
		else
		{
			InventorySlots[FromIndex].Amount = (GetAmountAtIndex(FromIndex) - (MaxStackSize - GetAmountAtIndex(ToIndex)));

			InventorySlots[ToIndex].ItemClass = InventorySlots[FromIndex].ItemClass;
			InventorySlots[ToIndex].Amount = MaxStackSize;

			OnUpdateRequest.Broadcast(FromIndex);
			OnUpdateRequest.Broadcast(ToIndex);
			

			return true;

		}
	}
	

	//OnUpdateRequest.Broadcast(FromIndex);
	//OnUpdateRequest.Broadcast(ToIndex);
	
	return false;
}

bool UInventoryComponent::SplitStackToIndex(int32 FromIndex, int32 ToIndex, int32 Amount)
{
	if (IsSlotEmpty(ToIndex) && !IsSlotEmpty(FromIndex) && GetItemInfoAtIndex(FromIndex).CanBeStacked && GetAmountAtIndex(FromIndex) > 1 && GetAmountAtIndex(FromIndex) > Amount)
	{
		
		InventorySlots[FromIndex].Amount = (InventorySlots[FromIndex].Amount - Amount);

		InventorySlots[ToIndex].ItemClass = InventorySlots[FromIndex].ItemClass;
		InventorySlots[ToIndex].Amount = Amount;

		OnUpdateRequest.Broadcast(FromIndex);
		OnUpdateRequest.Broadcast(ToIndex);

		return true;
	}
	else
	{
		return false;
	}
}

int32 UInventoryComponent::GetAmountAtIndex(int32 index) const
{
	return InventorySlots[index].Amount;
}

FItemInfo UInventoryComponent::GetItemInfoAtIndex(int32 index) const
{
	return InventorySlots[index].ItemClass.GetDefaultObject()->ItemInfo;
}



bool UInventoryComponent::isItemInInventory(TSubclassOf<ABaseItem> ItemClassToFind)
{
	int32 index = 0;
	while (index < InventorySlots.Num())
	{
		if ( InventorySlots[index].ItemClass == ItemClassToFind)
		{
			return true;
		}
		index++;
	}

	return false;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	//Set the size of array
	InventorySlots.SetNum(NumberOfSlots);
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

