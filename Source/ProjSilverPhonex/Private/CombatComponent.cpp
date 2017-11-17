// Copyright Project Silver Phoneix

#include "CombatComponent.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


EBattleState UCombatComponent::GetBattleState() const
{
	return CurrentBattleState;
}

void UCombatComponent::SetBattleState(EBattleState BattleState)
{
	CurrentBattleState = BattleState;
}

void UCombatComponent::KnockBack(ACharacter * DamageCauser, ACharacter * DamageReceiver)
{

	FVector Direction = DamageReceiver->GetActorLocation() - DamageCauser->GetActorLocation();
	FVector KnockbackDirection = ((Direction * FVector(1, 1, 0)) * KnockBackAmount) * FVector(1).Z;
	DamageReceiver->LaunchCharacter(KnockbackDirection, true, false);
}

void UCombatComponent::Flinch()
{
	ACharacter* Owner = Cast<ACharacter>(GetOwner());

	if (Owner)
	{
		IsFlinching = true;
		float Duration = 0.f; 
		//Play animation when hit
		if (FlinchAnimation)
		{
			Duration =  Owner->PlayAnimMontage(FlinchAnimation);
			//Stop from attacking and reset everything when hit
		}
		
		GetWorld()->GetTimerManager().SetTimer(FlinchHandleTimer, this, &UCombatComponent::StopFlinch, Duration, false);
	}
}

void UCombatComponent::StopFlinch()
{
	IsFlinching = false;
	GetWorld()->GetTimerManager().ClearTimer(FlinchHandleTimer);
}

bool UCombatComponent::GetIsFlinching() const
{
	return IsFlinching;
}




