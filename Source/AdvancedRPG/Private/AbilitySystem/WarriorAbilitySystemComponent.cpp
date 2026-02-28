// pravin's  patashala all Rights Reserved 


#include "AbilitySystem/WarriorAbilitySystemComponent.h"

void UWarriorAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTagToGive)
{
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!InInputTagToGive.IsValid())
		{
			return;
		}

		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTagToGive)) continue;
		TryActivateAbility(AbilitySpec.Handle);

	}
}

void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTagToGive)
{

}
