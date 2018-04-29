// Copyright Project Silver Phoneix

#include "CombatComponent.h"
#include "Animation/AnimMontage.h"
#include "XBaseCharacter.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	
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

void UCombatComponent::KnockBack(AActor * DamageCauser, AActor * DamageReceiver)
{
	if (DamageCauser == nullptr || DamageReceiver == nullptr)
	{
		return;
	}

	FVector Direction = DamageReceiver->GetActorLocation() - DamageCauser->GetActorLocation();
	FVector KnockbackDirection = ((Direction * FVector(1, 1, 0)) * KnockBackAmount) * FVector(2).Z;
	ACharacter* CharDamageReceiver = Cast<ACharacter>(DamageReceiver);
	if (CharDamageReceiver)
	{
		CharDamageReceiver->LaunchCharacter(KnockbackDirection, true, false);
	}
	
}

//void UCombatComponent::Reaction(const FHitResult HitResult)
//{
//	FVector hitpoint = HitResult.Location;
//	DrawDebugSphere(GetWorld(), hitpoint, 10.f, 14, FColor::Red, false, 1.f, 0, 1.f);
//	AXBaseCharacter* player = Cast<AXBaseCharacter>(HitResult.GetActor());
//	
//	//player->GetMesh()->GetComponentRotation().GetRightVector();
//	//FVector LookDir = HitResult.GetActor()->GetActorLocation() - (HitResult.GetActor()->GetActorLocation() + HitResult.GetActor()->GetActorRightVector());
//
//	return;
//	FVector LookDir = HitResult.GetActor()->GetActorRightVector();
//	float num = FVector::DotProduct(hitpoint.GetSafeNormal(), LookDir.GetSafeNormal());
//	float CrossNum = FVector::CrossProduct(hitpoint.GetSafeNormal(), LookDir.GetSafeNormal()).Z;
//
//	if (num >= -1.0f && num < -0.7f)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Back"));
//	}
//	else if (num >= 0.7 && num <= 1.0f)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Front"));
//
//	}
//	else if (num >= -0.7 && num <= 0.7)
//	{
//		if (CrossNum < 0)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Right"));
//		}
//		else if(CrossNum > 0)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Left"));
//		}
//
//	}
//	UE_LOG(LogTemp, Warning, TEXT("Direction %f : Crossnum: %f"), num, CrossNum);
//
//}
 
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




