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
	SetTargetIconDirection();


	/* Check if the last time we sensed a player is beyond the time out value to prevent from endlessly following a player. */
	if (bSensedTarget && (GetWorld()->TimeSeconds - LastSeenTime) > SenseTimeOut)
	{
		AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
		if (AIController)
		{
			UE_LOG(LogTemp, Warning, TEXT("Reset"))
			bSensedTarget = false;
			/* Reset */
			AIController->SetSeenTarget(nullptr);

		}
	}
}


//TODO - Pass a reference of the person who killed them 
void AEnemyMaster::OnDeath() 
{
	Super::OnDeath();


	USPGameInstance* GameInstance = Cast<USPGameInstance>(GetGameInstance());

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

	Destroy();
}

void AEnemyMaster::SetTargetIconDirection()
{
	auto* Player = Cast<AXBaseCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (Player)
	{
		auto Cameralocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
		auto IconLocation = TargetIcon->K2_GetComponentLocation();
		auto NewRotation = UKismetMathLibrary::FindLookAtRotation(IconLocation, Cameralocation);
		TargetIcon->SetWorldRotation(NewRotation);
	}

}

//TODO - Chnage name of function
void AEnemyMaster::SetTargetHidden(bool NewState)
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

		AIController->SetSeenTarget(pawn);

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

