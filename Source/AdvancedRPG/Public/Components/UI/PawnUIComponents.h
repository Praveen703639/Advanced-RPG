// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "PawnUIComponents.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPercentChangedDelegate, float, NewPercent);

/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UPawnUIComponents : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "UI")
	FOnPercentChangedDelegate OnCurrentHealthChnaged;
	
};
