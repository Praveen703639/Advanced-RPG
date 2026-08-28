// pravin's  patashala all Rights Reserved 


#include "AbilitySystem/Abilities/WarriorBowGameplayAbility.h"

#include "Characters/WarriorBowCharacter.h"
#include "Controllers/WarriorBowController.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"



AWarriorBowCharacter* UWarriorBowGameplayAbility::GetWarriorBowCharacterFromActorInfo()
{
	if (!CachedWarriorBowCharacter.IsValid())
	{
		CachedWarriorBowCharacter = Cast<AWarriorBowCharacter>(CurrentActorInfo->AvatarActor);
	}

	return CachedWarriorBowCharacter.IsValid() ? CachedWarriorBowCharacter.Get() : nullptr;
}

AWarriorBowController* UWarriorBowGameplayAbility::GetWarriorBowControllerFromActorInfo()
{
	if (!CachedWarriorBowController.IsValid())
	{
		CachedWarriorBowController = Cast<AWarriorBowController>(CurrentActorInfo->PlayerController);
	}

	return CachedWarriorBowController.IsValid() ? CachedWarriorBowController.Get() : nullptr;
}
