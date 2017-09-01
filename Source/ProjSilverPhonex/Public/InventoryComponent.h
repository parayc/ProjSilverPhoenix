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
	int32 GetAmountAtIndex(int32 index) const;
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FItemInfo GetItemInfoAtIndex(int32 index) const;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnUpdateRequest OnUpdateRequest;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
