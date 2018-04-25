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
	TargetIcon->SetupAttachment(GetMesh());
	TargetIcon->SetDrawSize(FVector2D(20, 15));
	TargetIcon->bVisible = true;
	
	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Widget"));
	HealthWidget->SetupAttachment(GetMesh());
	HealthWidget->SetDrawSize(FVector2D(60, 15));
	HealthWidget->SetVisibility(false);

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(40.f);
	PawnSensingComp->HearingThreshold = 600;//how far it can hear
	PawnSensingComp->LOSHearingThreshold = 1200;
	
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
	
}

// Called every frame
void AEnemyMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//SetTargetIconDirection();

	/* Check if the last time we sensed a player is beyond the time out value to prevent from endlessly following a player. */
	if (LastTimeSensePlayer())
	{
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

void AEnemyMaster::OnHealthChanged(UHealthComponent * OwningHealthComp, float Health, float HealthDelta, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
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

	if (!ensure(CombatStates)) { return; }

	if (CombatStates->GetBattleState() == EBattleState::PS_Normal)
	{
		//Reset combo if we get hit
		UMeleeAnimInstance* PlayerAnimation = Cast<UMeleeAnimInstance>(GetMesh()->GetAnimInstance());
		if (PlayerAnimation)
		{
			PlayerAnimation->ResetComboAttack();
		}

		CombatStates->KnockBack(DamageCauser, this);
		CombatStates->Flinch();

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

bool AEnemyMaster::LastTimeSensePlayer()
{

	return bSensedTarget && (GetWorld()->TimeSeconds - LastSeenTime) > SenseTimeOut ||
		GetIsDead() || (EnemyRef && EnemyRef->GetIsDead());
}

