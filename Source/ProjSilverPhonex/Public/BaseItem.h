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
class PROJSILVERPHONEX_API ABaseItem : public AActor
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

	//UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category = "UseItems")
	virtual void UseItem(AActor* Owner);

	UPROPERTY( BlueprintReadWrite, Category = "Inventory")
	UInventoryComponent* InventoryRef = nullptr;
	
	int32 Index;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
