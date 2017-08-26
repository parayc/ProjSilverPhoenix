// Copyright Project Silver Phoneix

#include "SPGameInstance.h"

int32 USPGameInstance::GetPlayersLevel() const
{
	return PlayersLevel;
}

int32 USPGameInstance::GetCurrentExp() const
{
	return expGained;
}

int32 USPGameInstance::GetExpNeeded() const
{
	return expNeeded;
}

void USPGameInstance::LevelUp()
{
	//Add the remainig exp to the new level
	int32 RemainingExp = expGained - expNeeded;
	UpdateExp();
	expGained = RemainingExp;
	PlayersLevel++;//Increase players level 
}

void USPGameInstance::UpdateExp()
{
	//Update exps bar
	expNeeded = pow(PlayersLevel, 2) * 200;
}

void USPGameInstance::AddExpPoints(int32 Exp)
{
	//Add exp to player current exp
	expGained += Exp;
	//Check if we reached next level
	if (expGained >= expNeeded)
	{
		LevelUp();
	}
}
