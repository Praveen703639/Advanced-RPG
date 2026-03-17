// pravin's  patashala all Rights Reserved 


#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"

void UWarriorAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTagToGive)
{
	// Validate input tag before loop - prevents unnecessary iterations
	if (!InInputTagToGive.IsValid())
	{
		return;
	}

	// Iterate through all activatable abilities to find matching input tag
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// Check if this ability has the matching input tag
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTagToGive))
			continue;

		// Ability found with matching tag, activate it
		TryActivateAbility(AbilitySpec.Handle);
	}
}

void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTagToGive)
{

}

void UWarriorAbilitySystemComponent::GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandle)
{
	// Return early if array is empty - prevents unnecessary processing
	if (InDefaultWeaponAbilities.IsEmpty()) 
		return;

	// Iterate through each weapon ability set
	for (const FWarriorHeroAbilitySet& AbilitSet : InDefaultWeaponAbilities) 
	{
		// Validate ability set before granting
		if (!AbilitSet.IsValid()) 
			continue;

		// Create ability spec from the ability class
		FGameplayAbilitySpec AbilitySpec(AbilitSet.AbilityToGrant);

		// Set the source object to the character owning this ability system
		AbilitySpec.SourceObject = GetAvatarActor();

		// Set the ability level
		AbilitySpec.Level = ApplyLevel;

		// Add the input tag using the modern API
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitSet.InputTag);

		// Grant the ability and store the returned handle for later removal if needed
		OutGrantedAbilitySpecHandle.AddUnique(GiveAbility(AbilitySpec));
	}
}

void UWarriorAbilitySystemComponent::RemoveGrantedHeroWeaponAbilities(UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	// Return early if array is empty - prevents unnecessary processing
	if (InSpecHandlesToRemove.IsEmpty())
		return;

	// Iterate through each ability spec handle and revoke the corresponding ability
	for (const FGameplayAbilitySpecHandle& SpecHandle : InSpecHandlesToRemove)
	{
		if (SpecHandle.IsValid())
		{
			ClearAbility(SpecHandle);
		}
	}

	// Clear the input array after processing to prevent stale handles
	InSpecHandlesToRemove.Empty();
}