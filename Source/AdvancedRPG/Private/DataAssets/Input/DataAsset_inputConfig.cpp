// pravin's  patashala all Rights Reserved 


#include "DataAssets/Input/DataAsset_inputConfig.h"

UInputAction* UDataAsset_inputConfig::FindNativeInputActionByTag(const FGameplayTag& InInputTag) const
{
	for (const FWarriorInputActionConfig& InputActionConfig : NativeInputActions)
	{
		if(InputActionConfig.InputTag == InInputTag)
		{
			return InputActionConfig.InputAction;
		}
	}
	return nullptr;
}
