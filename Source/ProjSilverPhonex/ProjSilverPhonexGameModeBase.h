// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjSilverPhonexGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJSILVERPHONEX_API AProjSilverPhonexGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	void MissionSuccess(bool isMissionSuccessful, AActor* PawnInstagor);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void OnMissionComplete(bool bMissionSuccess,AActor* InstagatorPawn);
protected:

	virtual void BeginPlay() override;
	
};
