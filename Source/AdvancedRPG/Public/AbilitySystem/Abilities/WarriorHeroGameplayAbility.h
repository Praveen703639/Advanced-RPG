// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "WarriorHeroGameplayAbility.generated.h"

class AWarriorHeroCharacter;
class AWarriorHeroController;
class UHeroCombatComponent;
class UWarriorWeaponBase;
class UWarriorBaseCharacter;

/**
 * UWarriorHeroGameplayAbility
 * 
 * Base class for all hero-specific gameplay abilities. Provides safe accessors to hero character
 * and controller while caching results for performance.
 * 
 * Key Features:
 * - Caching of frequently accessed actor references using TWeakObjectPtr for memory safety
 * - Blueprint-pure accessors for hero character, controller, and combat component
 * - Defensive programming with null checks on all accessors
 * 
 * Usage:
 * Inherit from this class for all hero abilities (combat abilities, equip abilities, etc.)
 * Use the getter functions to safely access the character and its components.
 * 
 * Example:
 * class UGA_HeroAttack : public UWarriorHeroGameplayAbility
 * {
 *     void ActivateAbility(...)
 *     {
 *         AWarriorHeroCharacter* HeroChar = GetWarriorHeroCharacterFromActorInfo();
 *         if (HeroChar)
 *         {
 *             HeroChar->GetHeroCombatComponent()->PerformAttack();
 *         }
 *     }
 * };
 */
UCLASS()
class ADVANCEDRPG_API UWarriorHeroGameplayAbility : public UWarriorGameplayAbility
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
	AWarriorHeroCharacter* GetWarriorHeroCharacterFromActorInfo();

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
	AWarriorHeroController* GetWarriorHeroControllerFromActorInfo();

	/**
	 * Gets the hero combat component safely with null checks.
	 * 
	 * Retrieves the combat component from the hero character. Includes defensive null checks
	 * to prevent crashes if the character is not available.
	 * 
	 * @return The UHeroCombatComponent if available, nullptr otherwise
	 * 
	 * @note This is a BlueprintPure function, safe to call from Blueprint
	 */
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UHeroCombatComponent* GetHeroCombatComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	FGameplayEffectSpecHandle MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, int32 InUsedComboCount);

	

	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	bool GetAbilityRemainingCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldownTime, float& RemainingCooldownTime);

	/**
	 * Gets the cooldown duration from the ability's assigned Cooldown Gameplay Effect.
	 * Reads the DurationMagnitude from the CDO of the CooldownGameplayEffectClass.
	 *
	 * @return The cooldown duration in seconds, or 0.f if no cooldown effect is set.
	 */
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	float GetCooldownDurationFromGE() const;

	
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UHeroUIComponent* GetHeroUIComponentFromActorInfo();

	//UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	//UWarriorWeaponBase* GetWarriorCloneHeroWeaponFromActorInfo();

private:

	/** Cached reference to the hero character, uses weak pointer to avoid circular references */
	TWeakObjectPtr<AWarriorHeroCharacter> CachedWarriorHeroCharacter;	

	/** Cached reference to the hero controller, uses weak pointer to avoid circular references */
	TWeakObjectPtr<AWarriorHeroController> CachedWarriorHeroController;
};
