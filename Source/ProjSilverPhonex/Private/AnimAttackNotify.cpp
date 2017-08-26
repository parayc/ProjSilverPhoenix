// Copyright Project Silver Phoneix

#include "AnimAttackNotify.h"
#include "MeleeAnimInstance.h"

void UAnimAttackNotify::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	UMeleeAnimInstance* PlayerAnimation = Cast<UMeleeAnimInstance>(MeshComp->GetAnimInstance());

	if (PlayerAnimation)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Notiify"));
		PlayerAnimation->SetAcceptNextCombo(true);
	}
}
