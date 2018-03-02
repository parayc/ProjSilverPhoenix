// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "SPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJSILVERPHOENIX_API ASPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

		ASPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

		/* Update the FOV */
		virtual void UpdateCamera(float DeltaTime) override;
	
public:

	void test();
	
};
