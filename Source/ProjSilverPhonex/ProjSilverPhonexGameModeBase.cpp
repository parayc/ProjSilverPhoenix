// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjSilverPhonexGameModeBase.h"
#include "Kismet/GameplayStatics.h"

void AProjSilverPhonexGameModeBase::MissionSuccess(bool isMissionSuccessful, AActor* PawnInstagor)
{
	if (PawnInstagor)
	{

		OnMissionComplete(isMissionSuccessful, PawnInstagor);
	}

	
	
}

void AProjSilverPhonexGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}
