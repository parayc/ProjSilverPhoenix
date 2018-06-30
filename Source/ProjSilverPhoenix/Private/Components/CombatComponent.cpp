 // Copyright Project Silver Phoneix

#include "CombatComponent.h"
#include "Animation/AnimMontage.h"
#include "XBaseCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIController.h"
#include "EnemyAIController.h"
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

void UCombatComponent::Flinch(const FHitResult& HitResult)
{
	
	if (GetBattleState() != EBattleState::PS_Normal) return;
	AXBaseCharacter* Owner = Cast<AXBaseCharacter>(GetOwner());
	if (Owner && !Owner->GetIsDead())
	{
	
		FVector HitPoint = HitResult.ImpactPoint;
		//DrawDebugSphere(GetWorld(), HitPoint, 10.f, 14, FColor::Yellow, false, 1.f, 0, 1.f);

		FVector ForwardDir = Owner->GetActorForwardVector();
		FVector Dir = Owner->GetActorLocation() - HitPoint;
		float DotNum = FVector::DotProduct(Dir.GetSafeNormal(), ForwardDir.GetSafeNormal());
		float CrossNum = FVector::CrossProduct(Dir.GetSafeNormal(), ForwardDir.GetSafeNormal()).Z;
		IsFlinching = true;
		float Duration = 0.f;

		if (DotNum >= -1.0f && DotNum < -0.7f)
		{
			if (FrontFlinchAnim)
			{
				Duration = Owner->PlayAnimMontage(FrontFlinchAnim);
			}
			
		}
		else if (DotNum >= 0.7 && DotNum <= 1.0f)
		{
			if (BackFlinchAnim)
			{
				Duration = Owner->PlayAnimMontage(BackFlinchAnim);
			}
	
		}
		else if (DotNum >= -0.7 && DotNum <= 0.7)
		{
			if (CrossNum < 0)
			{
				if (LeftFlinchAnim)
				{
					Duration = Owner->PlayAnimMontage(LeftFlinchAnim);
				}
				
			}
			else if (CrossNum > 0)
			{
				if (RightFlinchAnim)
				{
					Duration = Owner->PlayAnimMontage(RightFlinchAnim);
					
				}
	
			}

		}
		//UE_LOG(LogTemp, Warning, TEXT("Direction %f : Crossnum: %f"), DotNum, CrossNum);
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

void UCombatComponent::KnockDown(AActor* DamageInstigator)
{
	AXBaseCharacter* Owner = Cast<AXBaseCharacter>(GetOwner());
	float Duration = 0.f;
	if (Owner && !Owner->GetIsDead())
	{
		bIsKnockedDown = true;

		auto Direction(DamageInstigator->GetActorLocation() - Owner->GetActorLocation());
		auto LookDir = UKismetMathLibrary::MakeRotFromX(Direction);
		FRotator NewTarget(0, LookDir.Yaw, 0);
		Owner->SetActorRotation(NewTarget);
		
		if (KnockDownAnim)
		{
			Owner->PlayAnimMontage(KnockDownAnim);
		}
		
	}
	
}

void UCombatComponent::ResetKnockDown()
{

	AXBaseCharacter* Owner = Cast<AXBaseCharacter>(GetOwner());
	float Duration = 0.f;
	if (Owner && !Owner->GetIsDead())
	{
		if (KnockDownRecoveryAnim)
		{
			Owner->PlayAnimMontage(KnockDownRecoveryAnim);
		}
	
	}
}

void UCombatComponent::KnockDownEnd()
{
	bIsKnockedDown = false;
}

void UCombatComponent::CalculateKnockDown()
{
}

bool UCombatComponent::GetIsKnockedDown() const
{
	return bIsKnockedDown;
}




