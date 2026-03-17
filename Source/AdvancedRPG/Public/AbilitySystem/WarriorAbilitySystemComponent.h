// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "WarriorTypes/WarriorStructTypes.h"

#include "WarriorAbilitySystemComponent.generated.h"

/**
 * UWarriorAbilitySystemComponent
 * 
 * Specialized Ability System Component for Warrior game framework.
 * Extends UAbilitySystemComponent with custom input handling and weapon ability management.
 * 
 * Key Responsibilities:
 * - Handle ability activation through gameplay tags (OnAbilityInputPressed/Released)
 * - Grant weapon abilities dynamically when a weapon is equipped
 * - Track granted ability handles for cleanup when weapon is unequipped
 * 
 * Usage:
 * - Input system calls OnAbilityInputPressed with the input tag
 * - This searches for abilities with matching input tags and activates them
 * - Weapon systems call GrantHeroWeaponAbilities to add weapon-specific abilities
 */
UCLASS()
class ADVANCEDRPG_API UWarriorAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	/**
	 * Activates an ability based on an input tag.
	 * Called by the input system when a player presses an input button.
	 * 
	 * Searches through all activatable abilities for one with a matching input tag
	 * and attempts to activate it.
	 * 
	 * @param InInputTagToGive The input tag to search for (e.g., "Input.Attack.Light")
	 * 
	 * @see OnAbilityInputReleased for ability deactivation
	 */
	void OnAbilityInputPressed(const FGameplayTag& InInputTagToGive);

	/**
	 * Deactivates an ability based on an input tag.
	 * Called by the input system when a player releases an input button.
	 * 
	 * Can be used for abilities that require a held input.
	 * 
	 * @param InInputTagToGive The input tag to deactivate
	 */
	void OnAbilityInputReleased(const FGameplayTag& InInputTagToGive);

	/**
	 * Grants weapon abilities from a list of ability sets.
	 * Creates ability specs with the provided level and grants them to this ASC.
	 * 
	 * Stores the returned ability handles so they can be revoked later when
	 * the weapon is unequipped.
	 * 
	 * Design Pattern: Ability Set Management
	 * - Each weapon has a list of abilities (combos, special attacks, etc.)
	 * - When equipped, all weapon abilities are granted
	 * - The returned handles allow clean removal when unequipped
	 * 
	 * @param InDefaultWeaponAbilities Array of ability sets to grant
	 * @param ApplyLevel The level at which to grant the abilities (default 1)
	 * @param OutGrantedAbilitySpecHandle Array of ability spec handles that were granted (for later removal)
	 * 
	 * @see WarriorHeroCharacter::AddWeaponInputMappingContext for weapon input setup
	 */
	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability", meta = (ApplyLevel = "1"))
	void GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandle);

	/**
	 * Removes previously granted weapon abilities.
	 * Called when a weapon is unequipped to clean up its abilities.
	 * 
	 * Iterates through the ability handles and revokes each one, then clears the array.
	 * This ensures abilities are properly removed and handles don't become stale.
	 * 
	 * @param InSpecHandlesToRemove Array of ability spec handles to revoke
	 * 
	 * @see GrantHeroWeaponAbilities for ability granting
	 */
	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	void RemoveGrantedHeroWeaponAbilities(UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);

};
