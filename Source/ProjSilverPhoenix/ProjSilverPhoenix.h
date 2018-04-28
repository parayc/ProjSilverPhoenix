// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define ECC_Weapon ECC_GameTraceChannel1
#define ECC_TargetSystem ECC_GameTraceChannel2

UENUM(BlueprintType)
enum class EAIStates : uint8
{
	Passive,
	Suspicious,
	Alerted

};