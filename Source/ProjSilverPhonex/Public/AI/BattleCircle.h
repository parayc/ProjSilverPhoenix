// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattleCircle.generated.h"

class USphereComponent;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJSILVERPHONEX_API UBattleCircle : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBattleCircle();

	UPROPERTY(EditDefaultsOnly, Category = "SetUp")
		USphereComponent* BattleCircle;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
