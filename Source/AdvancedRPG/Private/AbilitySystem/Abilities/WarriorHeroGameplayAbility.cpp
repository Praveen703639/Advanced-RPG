// pravin's  patashala all Rights Reserved 


#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Controllers/WarriorHeroController.h"


AWarriorHeroCharacter* UWarriorHeroGameplayAbility::GetWarriorHeroCharacterFromActorInfo() 
{
	// Use weak pointer caching pattern to avoid repeated casts
	// On first call, this will be invalid and trigger a cast
	if (!CachedWarriorHeroCharacter.IsValid())
	{
		// Cast the avatar actor (character) from CurrentActorInfo
		// CurrentActorInfo is set when the ability is granted to an actor
		CachedWarriorHeroCharacter = Cast<AWarriorHeroCharacter>(CurrentActorInfo->AvatarActor);
	}

	// Return the cached character if valid, otherwise nullptr
	return CachedWarriorHeroCharacter.IsValid() ? CachedWarriorHeroCharacter.Get() : nullptr;
}

AWarriorHeroController* UWarriorHeroGameplayAbility::GetWarriorHeroControllerFromActorInfo()
{
	// Use weak pointer caching pattern to avoid repeated casts
	// On first call, this will be invalid and trigger a cast
	if (!CachedWarriorHeroController.IsValid())
	{
		// Cast the player controller from CurrentActorInfo
		// This is the controller that owns/possesses the avatar actor
		CachedWarriorHeroController = Cast<AWarriorHeroController>(CurrentActorInfo->PlayerController);
	}

	// Return the cached controller if valid, otherwise nullptr
	return CachedWarriorHeroController.IsValid() ? CachedWarriorHeroController.Get() : nullptr;
}

UHeroCombatComponent* UWarriorHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
	// Get the hero character with null check
	AWarriorHeroCharacter* HeroCharacter = GetWarriorHeroCharacterFromActorInfo();

	// Only attempt to get the combat component if the character is valid
	if (HeroCharacter)
	{
		return HeroCharacter->GetHeroCombatComponent();
	}

	// Return nullptr if character was not available
	return nullptr;
}
