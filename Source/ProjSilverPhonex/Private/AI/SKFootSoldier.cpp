// Copyright Project Silver Phoneix

#include "SKFootSoldier.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "BasePlayer.h"


void ASKFootSoldier::SetIsAttacking(bool NewState)
{
	bIsAttacking = bIsAttacking;
}

void ASKFootSoldier::BeginPlay()
{
	Super::BeginPlay();


		
		if (Weapon && DamageBox)
		{
			Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
			DamageBox->OnComponentBeginOverlap.AddDynamic(this, &ASKFootSoldier::OnPlayerEnterPickupBox);
		}
		
}

void ASKFootSoldier::OnPlayerEnterPickupBox(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFomSweep, const FHitResult & SweepResult)
{
	if (bIsAttacking)
	{
		if (auto Player = Cast<ABasePlayer>(OtherActor))
		{


			float DealtDamage = WeaponDamage;//Later maybe damage multipler 

			FPointDamageEvent DamageEvent;
			DamageEvent.Damage = DealtDamage;


			Player->TakeDamage(DealtDamage, DamageEvent, GetController(), this);
		}
	}
}

ASKFootSoldier::ASKFootSoldier()
{

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	Weapon->SetupAttachment(RootComponent);

	DamageBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Damage Box"));
	DamageBox->AttachToComponent(Weapon, FAttachmentTransformRules::KeepRelativeTransform);

	
	
}

