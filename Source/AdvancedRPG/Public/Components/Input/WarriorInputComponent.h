// pravin's patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "DataAssets/Input/DataAsset_inputConfig.h"
#include "WarriorInputComponent.generated.h"


UCLASS()
class ADVANCEDRPG_API UWarriorInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY() 

public:
	
	template<class UserObject, typename CallbackFunc>
	void BindNativeInputAction(const UDataAsset_inputConfig* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func);
};

template<class UserObject, typename CallbackFunc>
inline void UWarriorInputComponent::BindNativeInputAction(const UDataAsset_inputConfig* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func)
{
	checkf(InInputConfig, TEXT("Input Config is null! Forget to assign a valid Data Asset?"));

	if (UInputAction* FoundAction = InInputConfig->FindNativeInputActionByTag(InInputTag))
	{
		BindAction(FoundAction, TriggerEvent, ContextObject, Func);
	}
}