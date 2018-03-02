// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjSilverPhoenixGameModeBase.h"
#include "Kismet/GameplayStatics.h"

void AProjSilverPhoenixGameModeBase::MissionSuccess(bool isMissionSuccessful, AActor* PawnInstagor)
{
	if (PawnInstagor)
	{
		OnMissionComplete(isMissionSuccessful, PawnInstagor);
	}

}

void AProjSilverPhoenixGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}
