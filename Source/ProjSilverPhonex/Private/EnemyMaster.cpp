// Copyright Project Silver Phoneix

#include "EnemyMaster.h"
#include "BasePlayer.h"
#include "EnemyAIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/PawnSensingComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/WidgetComponent.h"
#include "CombatComponent.H"
#include "XBaseCharacter.h"


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


	PawningSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawningSensingComp->SetPeripheralVisionAngle(40.f);
	

	CombatStates = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat States"));

}

// Called when the game starts or when spawned
void AEnemyMaster::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHealth = MaxHealth;
	

	if (PawningSensingComp)
	{
		PawningSensingComp->OnSeePawn.AddDynamic(this, &AEnemyMaster::OnseePlayer);
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
}

bool AEnemyMaster::GetIsDead()
{
	return bIsDead;
}

//TODO - Pass a reference of the person who killed them 
void AEnemyMaster::OnDeath()
{
	auto* Player = Cast<ABasePlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());

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
		AIController->SetSeenTarget(pawn);

	}
}

