// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FrontEndTypes/FrontEndEnumTypes.h"
#include "FrontEndUISubsystem.generated.h"

class UWidget_PrimaryLayOut;
struct FGameplayTag;
class UWidget_ActivatableBase;
class UFrontendCommonButtonBase;

enum class EAsynPushWidgetState : uint8
{

	OnCreatedBeforePush,
	AfetrPush
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnButtonDescriptionTextUpdatedDelegate, UFrontendCommonButtonBase*, BroadCastingButton, FText, DescriptionText);

/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UFrontEndUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UFrontEndUISubsystem* Get(UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	UFUNCTION(BlueprintCallable)
	void RegisterCretaedPrimaryLayout(UWidget_PrimaryLayOut* CreatedLayout);

	void PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag, TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,TFunction<void(EAsynPushWidgetState, UWidget_ActivatableBase*)> AsyncPushStateCallback);

	void PushConfirmScreenToModalStackAsync(EConfirmScreenType InScreenType, const FText& InScreenTitle, const FText& InScreenMessage, TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallBack);

	UPROPERTY(BlueprintAssignable)
	FOnButtonDescriptionTextUpdatedDelegate OnButtonDescriptionTextUpdatedDelegate;

private:
	UPROPERTY(Transient)
	UWidget_PrimaryLayOut* CreatedPrimaryLayout;

	
};
