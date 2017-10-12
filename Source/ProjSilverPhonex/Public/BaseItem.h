// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

class UInventoryComponent;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItemRequest);

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
	UTexture2D* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText UseText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanBeUsed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanBeStacked;



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

	void UseItem();

	UPROPERTY( BlueprintReadWrite, Category = "Inventory")
	UInventoryComponent* InventoryRef = nullptr;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ItemStaticMesh;
	UPROPERTY(VisibleAnywhere)
	USphereComponent* PickUpSphere;

	int32 Index;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
