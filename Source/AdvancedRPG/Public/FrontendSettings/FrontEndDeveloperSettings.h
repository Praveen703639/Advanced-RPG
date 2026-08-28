// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "FrontEndDeveloperSettings.generated.h"

class UWidget_ActivatableBase;
/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = " FrontEnd UI Settings "))
class ADVANCEDRPG_API UFrontEndDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Widget Reference ", meta = (ForceInLineRow,Categories = "FrontEnd.Widget"))
	TMap<FGameplayTag, TSoftClassPtr<UWidget_ActivatableBase> > FrontEndWidgetMap;

	
};
