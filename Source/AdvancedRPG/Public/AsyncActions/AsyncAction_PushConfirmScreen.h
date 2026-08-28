// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "FrontEndTypes/FrontEndEnumTypes.h"
#include "AsyncAction_PushConfirmScreen.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConfirmScreenButtonClickedDelegate, EConfirmScreenButtonType, ClickedButtonType);
/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UAsyncAction_PushConfirmScreen : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject", BlueprintInternalUseOnly = "true", DisplayName = "Show Confirmation Screen"))
	static UAsyncAction_PushConfirmScreen* PushConfirmScreen(
		const UObject* WorldContextObject, 
		EConfirmScreenType InScreenType,
		FText InScreenTitle,
		FText InScreenMessage);

	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable)
	FOnConfirmScreenButtonClickedDelegate OnButtonClicked;

private:
	TWeakObjectPtr<UWorld> CachedOwningWorld;
	EConfirmScreenType  CachedScreenType;
	FText CachedScreenTitle;
	FText CachedScreenMessage;


	
};
