// Copyright Project Silver Phoneix

#include "AnimResetNotify.h"
#include "MeleeAnimInstance.h"

void UAnimResetNotify::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	UMeleeAnimInstance* PlayerAnimation = Cast<UMeleeAnimInstance>(MeshComp->GetAnimInstance());
	if (PlayerAnimation)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Reset Combo"));
		PlayerAnimation->SetAcceptNextCombo(false);
		PlayerAnimation->ComboReset();
	}
}
