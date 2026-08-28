// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "FrontendtFunctionLibrary.generated.h"
class UWidget_ActivatableBase;
/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UFrontendtFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Frontend Function Library")
	static TSoftClassPtr<UWidget_ActivatableBase> GetFrontEndSoftWidgetClassByTag(UPARAM(meta = (Categories = " FrontEnd.Widget ")) FGameplayTag InWidgetTag);
	
};
