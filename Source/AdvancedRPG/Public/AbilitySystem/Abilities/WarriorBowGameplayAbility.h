// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "WarriorBowGameplayAbility.generated.h"

class AWarriorBowCharacter;
class AWarriorBowController;
class UHeroCombatComponent;




/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UWarriorBowGameplayAbility : public UWarriorGameplayAbility
{
	GENERATED_BODY()

protected:

	/**
	 * Gets the hero character safely, caching the result for performance.
	 *
	 * Uses weak pointer caching to avoid repeated casts. On first call, casts CurrentActorInfo->AvatarActor
	 * to AWarriorHeroCharacter and caches the result. Subsequent calls return the cached value.
	 *
	 * @return The AWarriorHeroCharacter if valid, nullptr otherwise
	 *
	 * @note This is a BlueprintPure function, safe to call from Blueprint
	 */
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	AWarriorBowCharacter* GetWarriorBowCharacterFromActorInfo();

	/**
	 * Gets the hero controller safely, caching the result for performance.
	 *
	 * Uses weak pointer caching to avoid repeated casts. On first call, casts CurrentActorInfo->PlayerController
	 * to AWarriorHeroController and caches the result. Subsequent calls return the cached value.
	 *
	 * @return The AWarriorHeroController if valid, nullptr otherwise
	 *
	 * @note This is a BlueprintPure function, safe to call from Blueprint
	 */

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	AWarriorBowController* GetWarriorBowControllerFromActorInfo();	

private:
	// Cached weak pointers to avoid repeated casts
	TWeakObjectPtr<AWarriorBowCharacter> CachedWarriorBowCharacter;
	TWeakObjectPtr<AWarriorBowController> CachedWarriorBowController;

	
};

