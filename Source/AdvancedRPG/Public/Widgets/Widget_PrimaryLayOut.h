// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "Widget_PrimaryLayOut.generated.h"

class UCommonActivatableWidgetContainerBase;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class ADVANCEDRPG_API UWidget_PrimaryLayOut : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UCommonActivatableWidgetContainerBase* FindWidgetStackByTag(const FGameplayTag& InTag) const;

protected:
	UFUNCTION(BlueprintCallable)
	void RegisterWidgetStack(UPARAM(meta = (Categories = "FrontEnd.WidgetStack")) FGameplayTag InstackTag, UCommonActivatableWidgetContainerBase* InStack);

private:
	UPROPERTY(Transient)
	TMap<FGameplayTag, UCommonActivatableWidgetContainerBase*> RegisteredWidgetStackMap;
};
