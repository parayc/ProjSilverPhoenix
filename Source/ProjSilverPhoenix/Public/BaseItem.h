// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

class UInventoryComponent;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItemRequest);


UENUM(BlueprintType)
enum class EItemTypes : uint8
{
	ET_Consumable UMETA(DisplayName = "Consumable"),
	ET_QuestItem UMETA(DisplayName = "Quest Item"),
	ET_Weapon UMETA(DisplayName = "Weapon")

};
/*Contain Inofrmation on what the item is */
USTRUCT(BlueprintType)
struct FItemInfo
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemTypes ItemTpe;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText UseText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanBeUsed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanBeStacked;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMesh* PickUpMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector MeshScale = FVector(1.1);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float InteractRadius = 160;



};

UCLASS()
class PROJSILVERPHOENIX_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	FItemInfo ItemInfo;

	//UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category= "Item")

	UPROPERTY(BlueprintAssignable)
	FOnUseItemRequest OnUseItemRequest;

	//This is called 
	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
	void OnActivited(AActor* OwnerController, UInventoryComponent* InventoryRef);

	//This is called 
	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
	void OnExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
	void OnItemTick();

	virtual void UseItem(AActor* OwnerController);

	UPROPERTY( BlueprintReadWrite, Category = "Inventory")
	UInventoryComponent* InventoryRef = nullptr;
	
	int32 Index;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTickPowerup();

	/*Time between ticks - if Tick interval is less than or equal to one, item wont tick*/
	UPROPERTY(EditDefaultsOnly, category = "Powerurps")
	float TickInterval;

	/*Total times we apply the power up affact*/
	UPROPERTY(EditDefaultsOnly, category = "Powerurps")
	int32 TotalNumberOfTicks;

	FTimerHandle TimerHandle_ItemTick;

	/*Total number of ticks applied*/
	int32 TicksCounter;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
