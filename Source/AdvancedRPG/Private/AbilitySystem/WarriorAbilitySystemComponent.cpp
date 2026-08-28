// pravin's patashala all Rights Reserved 

#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "WarriorGameplayTags.h"

void UWarriorAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTagToGive)
{
	if (!InInputTagToGive.IsValid())
	{
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTagToGive))
		{
			continue;
		}

		if (InInputTagToGive.MatchesTag(WarriorGameplayTags::Input_Togglelable) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
		else
		{
			TryActivateAbility(AbilitySpec.Handle);
		}
	}
}

void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTagToGive)
{
	if (!InInputTagToGive.IsValid() || !InInputTagToGive.MatchesTag(WarriorGameplayTags::Input_MustBeHeld))
	{
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTagToGive) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
	}
}

void UWarriorAbilitySystemComponent::GrantHeroWeaponAbilities(
	const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities,
	const TArray<FWarriorHeroWeaponSpecialAbilitySet>& InSpecialWeaponAbilities,
	int32 ApplyLevel,
	TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
{
	auto GrantAbility = [&](const FWarriorHeroAbilitySet& AbilitySet) -> void
		{
			if (!AbilitySet.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("GrantHeroWeaponAbilities: Invalid ability set (Tag=%s)"), *AbilitySet.InputTag.ToString());
				return;
			}

			FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant.GetDefaultObject(), ApplyLevel);
			AbilitySpec.SourceObject = GetAvatarActor();
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);

			FGameplayAbilitySpecHandle Handle = GiveAbility(AbilitySpec);
			OutGrantedAbilitySpecHandles.AddUnique(Handle);

			UE_LOG(LogTemp, Log, TEXT("GrantHeroWeaponAbilities: Granted '%s' (Tag=%s)"),
				*AbilitySet.AbilityToGrant->GetName(),
				*AbilitySet.InputTag.ToString());
		};

	for (const FWarriorHeroAbilitySet& AbilitySet : InDefaultWeaponAbilities)
	{
		GrantAbility(AbilitySet);
	}

	for (const FWarriorHeroWeaponSpecialAbilitySet& AbilitySet : InSpecialWeaponAbilities)
	{
		GrantAbility(AbilitySet);
	}
}

void UWarriorAbilitySystemComponent::RemoveGrantedHeroWeaponAbilities(UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	if (InSpecHandlesToRemove.IsEmpty())
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : InSpecHandlesToRemove)
	{
		if (SpecHandle.IsValid())
		{
			ClearAbility(SpecHandle);
		}
	}

	InSpecHandlesToRemove.Empty();
}

bool UWarriorAbilitySystemComponent::TryActivateAbilityByInputTag(FGameplayTag InInputTagToGive)
{
	if (!InInputTagToGive.IsValid())
	{
		return false;
	}

	TArray<FGameplayAbilitySpec*> FoundAbilitySpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(InInputTagToGive.GetSingleTagContainer(), FoundAbilitySpecs);

	if (FoundAbilitySpecs.IsEmpty())
	{
		return false;
	}

	const int32 RandomIndex = FMath::RandRange(0, FoundAbilitySpecs.Num() - 1);
	FGameplayAbilitySpec* SpecToActivate = FoundAbilitySpecs[RandomIndex];

	if (SpecToActivate && !SpecToActivate->IsActive())
	{
		return TryActivateAbility(SpecToActivate->Handle);
	}

	return false;
}