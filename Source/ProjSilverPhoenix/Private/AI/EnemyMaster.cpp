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


// Sets default values
AEnemyMaster::AEnemyMaster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	TargetIcon = CreateDefaultSubobject<UWidgetComponent>(TEXT("Target Icon"));
	TargetIcon->SetupAttachment(GetMesh());
	TargetIcon->SetDrawSize(FVector2D(20, 15));
	TargetIcon->bVisible = true;
	
	TargetIcon->SetHiddenInGame(false);


	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(40.f);
	PawnSensingComp->HearingThreshold = 600;//how far it can hear
	PawnSensingComp->LOSHearingThreshold = 1200;
	

	CombatStates = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat States"));

}

// Called when the game starts or when spawned
void AEnemyMaster::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHealth = MaxHealth;
	


	if (PawnSensingComp)
	{

		PawnSensingComp->OnHearNoise.AddDynamic(this, &AEnemyMaster::OnHearNoise);
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemyMaster::OnseePlayer);
		
		
	}
	
	if (CombatStates)
	{
		CombatStates->SetBattleState(EBattleState::PS_Normal);
	}

	if (TargetIcon)
	{
		TargetIcon->SetHiddenInGame(true);
	}
	
}

// Called every frame
void AEnemyMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//SetTargetIconDirection();

	/* Check if the last time we sensed a player is beyond the time out value to prevent from endlessly following a player. */
	if (bSensedTarget && (GetWorld()->TimeSeconds - LastSeenTime) > SenseTimeOut || GetIsDead())
	{
		AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
		if (AIController)
		{
			bSensedTarget = false;
			/* Reset */
			AIController->SetSeenTarget(nullptr);
			//UE_LOG(LogTemp, Warning, TEXT("Set enemy nullptr"));
			

		}
	}

	
}



void AEnemyMaster::OnDeath() 
{
	Super::OnDeath();

	USPGameInstance* GameInstance = Cast<USPGameInstance>(GetGameInstance());

	//Remove the enemies collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	if (GameInstance)
	{
		GameInstance->AddExpPoints(Exp);
	}

	auto* Player = Cast<ASPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (Player && Player->GetIsLockedOn())
	{
		Player->LockOff();
		Player->FindLockOnTargets();
	}

	TimeSinceLastDeath = GetWorld()->TimeSeconds;

	DetachFromControllerPendingDestroy();
	SetLifeSpan(5.f);
}


//TODO - Chnage name of function
void AEnemyMaster::SetTargetIconVisibility(bool NewState)
{
	TargetIcon->SetHiddenInGame(NewState);
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
			AIController->SetSeenTarget(pawn);
		}
		else
		{
			AIController->SetSeenTarget(nullptr);
		
		}
		

	}
}

void AEnemyMaster::OnHearNoise(APawn * PawnInstigator, const FVector & Location, float Volume)
{
	
	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
	if (AIController)
	{
		AIController->SetHeardLocation(Location);

	}
}

