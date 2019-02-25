// Copyright Project Silver Phoneix

#include "BaseWeapon.h"
#include "XBaseCharacter.h"
#include "SPlayerController.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"



// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	

}


// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}


void ABaseWeapon::UseItem(AActor* OwnerController)
{
	Super::UseItem(OwnerController);
	
	auto controller = Cast<ASPlayerController>(OwnerController);
	if (controller)
	{
		auto Player = Cast<AXBaseCharacter>(controller->GetPawn());
		if (Player)
		{
			SetOwningPawn(Player);
			Player->AddWeaponToCharacterEquipment(this);
		}
	}

}

void ABaseWeapon::SetOwningPawn(AXBaseCharacter* NewOwner)
{
	//Check if not already the owner
	if (MyPawn != NewOwner)
	{
		Instigator = NewOwner;
		MyPawn = NewOwner;
		SetOwner(NewOwner);
	}

}

float ABaseWeapon::PlayWeaponAnimation(UAnimMontage * Animation, float InPlayRate, FName StartSectionName)
{
	float Duration = 0.0f;
	if (MyPawn)
	{
		if (Animation)
		{
			Duration = MyPawn->PlayAnimMontage(Animation, InPlayRate, StartSectionName);
		}
	}

	return Duration;
}

void ABaseWeapon::UnEquip()
{
	
	float Duration = PlayWeaponAnimation(UnEquipAnimation);

	if (Duration <= 0.f)
	{
		Duration = NoEquipAnimDuration;
	}

	//Once the animation has finish playing allow the player to attack
	GetWorldTimerManager().SetTimer(EquipFinishedTimerHandle, this, &ABaseWeapon::EquipingFinished, Duration, false);

}

void ABaseWeapon::EquipingFinished()
{
	//Allow the player to attack once animation is over 
	MyPawn->SetCanAttack(true);
	GetWorldTimerManager().ClearTimer(EquipFinishedTimerHandle);
}

void ABaseWeapon::DestroyWeapon()
{
	this->Destroy();
}

