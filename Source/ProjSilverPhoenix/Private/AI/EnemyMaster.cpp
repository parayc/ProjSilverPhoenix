// Copyright Project Silver Phoneix

#include "EnemyMaster.h"
#include "EnemyAIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/PawnSensingComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/WidgetComponent.h"
#include "CombatComponent.H"
#include "XBaseCharacter.h"
#include "SPlayer.h"
#include "SPGameInstance.h"
#include "Components/CapsuleComponent.h"
#include "ProjSilverPhoenixGameModeBase.h"
#include "HealthComponent.h"
#include "MeleeAnimInstance.h"


// Sets default values
AEnemyMaster::AEnemyMaster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	TargetIcon = CreateDefaultSubobject<UWidgetComponent>(TEXT("Target Icon"));
	TargetIcon->SetupAttachment(RootComponent);
	TargetIcon->SetDrawSize(FVector2D(20, 15));
	TargetIcon->bVisible = true;
	
	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Widget"));
	HealthWidget->SetupAttachment(RootComponent);
	HealthWidget->SetDrawSize(FVector2D(60, 15));
	HealthWidget->SetVisibility(false);

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(40.f);
	PawnSensingComp->HearingThreshold = 600;//how far it can hear
	PawnSensingComp->LOSHearingThreshold = 1200;

	AIStates = EAIStates::Passive;
	
}

// Called when the game starts or when spawned
void AEnemyMaster::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent = this->FindComponentByClass<UHealthComponent>();

	if (HealthComponent)
	{
		HealthComponent->OnHealthChange.AddDynamic(this, &AEnemyMaster::OnHealthChanged);
	}
	
	if (PawnSensingComp)
	{
		PawnSensingComp->OnHearNoise.AddDynamic(this, &AEnemyMaster::OnHearNoise);
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemyMaster::OnseePlayer);
	}

	if (TargetIcon)
	{
		TargetIcon->SetHiddenInGame(true);
	}

	SetGuardState(AIStates);
	
}

// Called every frame
void AEnemyMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//SetTargetIconDirection();

	/* Check if the last time we sensed a player is beyond the time out value to prevent from endlessly following a player. */
	if (LastTimeSensePlayer())
	{
		SetGuardState(EAIStates::Passive);

		AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
		if (AIController)
		{
			if (HealthWidget)
			{
				HealthWidget->SetVisibility(false);
			}
			
			bSensedTarget = false;
			/* Reset */
			AIController->SetSeenTarget(nullptr);
		}
	}

}

void AEnemyMaster::OnHealthChanged(UHealthComponent * OwningHealthComp, float Health, float HealthDelta, FVector HitDirection, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	//If We heal or do no damage just return
	if (HealthDelta <= 0) { return; }

	if (HealthWidget)
	{
		HealthWidget->SetVisibility(true);
	}

	if (Health <= 0 && !GetIsDead())
	{
		//Death
		SetIsDead(true);
		OnDeath();
		return;

	}

	/*Material Pulses when Ai takes damage  */

	//We make sure all material components pulses not just the base 
	if (MatInstList.Num() == 0)
	{
		for (int i = 0; i < GetMesh()->GetNumMaterials(); i++)
		{
			MatInstList.Add(GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(i, GetMesh()->GetMaterial(i)));
		}
	}


	if (MatInstList.Num() > 0)
	{

		for (int i = 0; i < MatInstList.Num(); i++)
		{
			MatInstList[i]->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
		
		}

	}
	
	/*This sets a varible in the AI behaviour tree */
	SetIsDamaged(true);


	if (!ensure(CombatStates)) { return; } 
	
	UMeleeAnimInstance* PlayerAnimation = Cast<UMeleeAnimInstance>(GetMesh()->GetAnimInstance());
	if (PlayerAnimation)
	{
		PlayerAnimation->ResetComboAttack();
	}

	if (CombatStates->GetBattleState() == EBattleState::PS_Normal)
	{
		CombatStates->HandleDamage(HealthDelta,HitDirection, InstigatedBy->GetPawn(),this);
	}
	else if(CombatStates->GetBattleState() == EBattleState::PS_SuperArmor && CombatStates->CalculateSuperArmor(HealthDelta))
	{
		CombatStates->HandleDamage(HealthDelta, HitDirection, InstigatedBy->GetPawn(), this);
	}
}

void AEnemyMaster::SetGuardState(EAIStates NewState)
{
	if (NewState == AIStates) return;

	AIStates = NewState;

	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
	if (AIController)
	{
		AIController->SetAIState(AIStates);
	}
}

void AEnemyMaster::OnDeath() 
{
	Super::OnDeath();

	if (HealthWidget)
	{
		HealthWidget->SetHiddenInGame(true);
	}

	USPGameInstance* GameInstance = Cast<USPGameInstance>(GetGameInstance());

	//Remove the enemies collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	if (GameInstance)
	{
		GameInstance->AddExpPoints(Exp);
	}

	auto* Player = Cast<ASPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());

	//TODO - check if we are lock on to this actor 
	if (Player && Player->GetIsLockedOn())
	{
		Player->LockOff();
		Player->RemoveEnemyFromTargeting(this);
		
	}

	TimeSinceLastDeath = GetWorld()->TimeSeconds;

	DetachFromControllerPendingDestroy();
	SetLifeSpan(5.f);
}

void AEnemyMaster::SetTargetIconHidden(bool NewState)
{
	TargetIcon->SetHiddenInGame(NewState);
}

void AEnemyMaster::SetIsDamaged(bool newState)
{
	bIsDamaged = newState;
	//Update in the Behaviour tree also
	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
	if (AIController)
	{
		AIController->SetIsDamagedState(bIsDamaged);
	}
}


void AEnemyMaster::OnseePlayer(APawn * pawn)
{
	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
	if (AIController)
	{
		/* Keep track of the time the player was last sensed in order to clear the target */
		LastSeenTime = GetWorld()->GetTimeSeconds();
		bSensedTarget = true;
		
		ASPlayer* player = Cast<ASPlayer>(pawn);
		if (player && !player->GetIsDead() && GetIsDead() == false)
		{
			EnemyRef = player;
			AIController->SetSeenTarget(EnemyRef);
			SetGuardState(EAIStates::Alerted);
		}
		else
		{
			AIController->SetSeenTarget(nullptr);
		}
		
	}
}

void AEnemyMaster::OnHearNoise(APawn * PawnInstigator, const FVector & Location, float Volume)
{
	if (AIStates == EAIStates::Alerted)
	{
		return;
	}

	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
	if (AIController)
	{
		AIController->SetHeardLocation(Location);
	}

	SetGuardState(EAIStates::Suspicious);
}

bool AEnemyMaster::LastTimeSensePlayer()
{

	return bSensedTarget && (GetWorld()->TimeSeconds - LastSeenTime) > SenseTimeOut ||
		GetIsDead() || (EnemyRef && EnemyRef->GetIsDead());
}

