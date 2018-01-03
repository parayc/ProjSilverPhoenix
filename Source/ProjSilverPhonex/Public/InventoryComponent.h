// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseItem.h"
#include "InventoryComponent.generated.h"


class ABaseItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateRequest,int32,Index);

USTRUCT(BlueprintType)
struct FInventorySlots
{
	GENERATED_BODY()

	FInventorySlots()
	{
		ItemClass = nullptr;

		Amount = 0;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ABaseItem> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJSILVERPHONEX_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	//Array of InventorySlots

	UPROPERTY(EditAnywhere, Category = "Inventory")
		TArray<FInventorySlots> InventorySlots;

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	//Array of InventorySlots
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<FInventorySlots> GetInventorySlots() const;
	/*The Number of items the inventory can hold*/

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetNumberOfSlots() const;

	UPROPERTY(EditAnywhere)
	int32 NumberOfSlots = 10;

	UPROPERTY(EditAnywhere)
	int32 MaxStackSize = 50;

	void AddItem(TSubclassOf<class ABaseItem> Item, int32 AmountToAdd);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool IsSlotEmpty(int32 Index);

	int32 SearchEmptySlot();

	int32 SearchFreeStack(TSubclassOf<class ABaseItem> Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveItemFromIndex(int32 Index, int32 AmountToRemove);

	//Swaps two inventory slots when one item slot is dragged over another 
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SwapSlots(int32 Index1, int32 Index2);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SplitStacks(int32 Index, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UseItemAtIndex(int32 Index);

	/*When an item is dragged on to the same item it check if i can be added to the stack*/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool AddToIndex(int32 FromIndex, int32 ToIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool SplitStackToIndex(int32 FromIndex, int32 ToIndex, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetAmountAtIndex(int32 index) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FItemInfo GetItemInfoAtIndex(int32 index) const;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnUpdateRequest OnUpdateRequest;
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool isItemInInventory(TSubclassOf<ABaseItem> ItemClassToFind);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
