// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimResetNotify.generated.h"

/**
 * 
 */
UCLASS()
class PROJSILVERPHONEX_API UAnimResetNotify : public UAnimNotify
{
	GENERATED_BODY()
	
	
		virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
