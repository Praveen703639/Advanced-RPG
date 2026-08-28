// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Widget_ActivatableBase.generated.h"
class AFrontEndController;


/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class ADVANCEDRPG_API UWidget_ActivatableBase : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintPure)
	AFrontEndController* GetOwningFrontEndController();


private:
	TWeakObjectPtr<AFrontEndController> CachedFrontEndOwningPC;

	
};
