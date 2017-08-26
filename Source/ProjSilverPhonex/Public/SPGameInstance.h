// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SPGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJSILVERPHONEX_API USPGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
private:
	int32 PlayersLevel = 1;

	int32 expGained;

	int32 expNeeded = 100;

	int32 MaxPlayerLevel;

public:
	UFUNCTION(BlueprintCallable, Category = "Setup")
		int32 GetPlayersLevel() const;
	UFUNCTION(BlueprintCallable, Category = "Setup")
		int32 GetCurrentExp() const;
	UFUNCTION(BlueprintCallable, Category = "Setup")
		int32 GetExpNeeded() const;

	void LevelUp();

	void UpdateExp();

	void AddExpPoints(int32 Exp);
	
	
};
