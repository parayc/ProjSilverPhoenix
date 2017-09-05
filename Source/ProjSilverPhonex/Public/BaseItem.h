// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

class UInventoryComponent;
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

	virtual void OnUseItem();

	UInventoryComponent* InventoryRef = nullptr;

	int32 Index;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
