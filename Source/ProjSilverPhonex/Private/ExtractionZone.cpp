// Copyright Project Silver Phoneix

#include "ExtractionZone.h"
#include "SPlayer.h"
#include "SPlayerController.h"
#include "Components/BoxComponent.h"
#include "InventoryComponent.h"
#include "ProjSilverPhonexGameModeBase.h"

// Sets default values
AExtractionZone::AExtractionZone()
{

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	RootComponent = BoxComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComp->SetupAttachment(RootComponent);
	
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AExtractionZone::OnBoxOverlapped);

}

// Called when the game starts or when spawned
void AExtractionZone::BeginPlay()
{
	Super::BeginPlay();
	
}

void AExtractionZone::OnBoxOverlapped(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	auto PC = Cast<ASPlayerController>(OtherActor->GetInstigatorController());
	if (PC)
	{
		auto Inventory = PC->FindComponentByClass<UInventoryComponent>();
		if (Inventory)
		{
			if (Inventory->isItemInInventory(ItemToFind))
			{
				auto GM = Cast<AProjSilverPhonexGameModeBase>(GetWorld()->GetAuthGameMode());
				if (GM)
				{
					GM->MissionSuccess(true, OtherActor);
				}
			}
		
		}
	}
}

