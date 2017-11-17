// Copyright Project Silver Phoneix

#include "AnimResetNotify.h"
#include "MeleeAnimInstance.h"

void UAnimResetNotify::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	UMeleeAnimInstance* PlayerAnimation = Cast<UMeleeAnimInstance>(MeshComp->GetAnimInstance());
	if (PlayerAnimation)
	{
		PlayerAnimation->ResetComboAttack();
	}
}
