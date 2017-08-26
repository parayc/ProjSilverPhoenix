// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimAttackNotify.generated.h"

/**
 * 
 */
UCLASS()
class PROJSILVERPHONEX_API UAnimAttackNotify : public UAnimNotify
{
	GENERATED_BODY()
	
		virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	
};
