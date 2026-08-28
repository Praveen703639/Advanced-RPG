// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FrontEndController.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API AFrontEndController : public APlayerController
{
	GENERATED_BODY()

protected:

	//~ Begin APlayerController Interface
	virtual void OnPossess(APawn* aPawn) override;
	//~ End APlayerController Interface
};
