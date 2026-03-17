// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "WarriorTypes/WarriorStructTypes.h"

#include "DataAsset_HeroStartUpData.generated.h"

class UWarriorGameplayAbility;
class UWarriorAbilitySystemComponent;

/**
 * UDataAsset_HeroStartUpData
 * 
 * Configuration data asset that defines what abilities a hero character should have at startup.
 * This implements the data-driven design pattern, allowing designers to configure starting abilities
 * without touching C++ code.
 * 
 * Design Pattern: Data-Driven Initialization
 * - Designers create instances of this asset and assign starting abilities via Blueprint editor
 * - At runtime, when the hero is possessed, this data is loaded and abilities are granted
 * - Abilities are granted through the Ability System Component with input binding
 * 
 * Usage:
 * 1. Create a data asset instance in Blueprint (e.g., DA_Hero_StartUpAbilities)
 * 2. Add ability sets to HeroStartUpAbilitySets array
 * 3. Assign this data asset to the hero character's CharacterStartUpData property
 * 4. When the hero is possessed, GiveToAbilitySystemComponent is automatically called
 * 
 * Example Setup:
 * - HeroStartUpAbilitySets[0]: FGameplayTag("Ability.Attack.Basic"), GA_BasicAttack
 * - HeroStartUpAbilitySets[1]: FGameplayTag("Ability.Defense.Block"), GA_Block
 * - HeroStartUpAbilitySets[2]: FGameplayTag("Ability.Special.Dash"), GA_Dash
 * 
 * Key Benefits:
 * - No code changes needed for different hero ability loadouts
 * - Easy to create variations (beginner abilities vs advanced abilities)
 * - Clear separation between ability definition and character logic
 * 
 * @see FWarriorHeroAbilitySet for the structure of individual ability entries
 * @see AWarriorHeroCharacter::PossessedBy for where this data is loaded
 */
UCLASS()
class ADVANCEDRPG_API UDataAsset_HeroStartUpData : public UDataAsset_StartUpDataBase
{
	GENERATED_BODY()

public:

	/**
	 * Grants all startup abilities from this data asset to the specified ability system component.
	 * 
	 * This function is called by AWarriorHeroCharacter::PossessedBy to initialize the character's abilities.
	 * It iterates through all ability sets, validates them, and grants each valid ability.
	 * 
	 * Implementation Details:
	 * - Calls parent class GiveToAbilitySystemComponent first for base setup
	 * - Iterates through HeroStartUpAbilitySets array
	 * - Validates each ability set before granting
	 * - Creates FGameplayAbilitySpec with proper source object and level
	 * - Binds input tags for input-triggered abilities
	 * - Uses GetDynamicSpecSourceTags() for modern API compliance
	 * 
	 * @param InASCToGive The ability system component that will receive the abilities
	 * @param ApplyLevel The ability level to grant (default 1). Higher levels may modify ability behavior.
	 * 
	 * @see FWarriorHeroAbilitySet::IsValid() for validation logic
	 * @see FGameplayAbilitySpec for ability configuration
	 */
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1) override;

	/**
	 * Spawns and registers all startup weapons for this hero.
	 * Called during hero initialization to create weapon actors and add them to the combat component.
	 * 
	 * @param InOwnerCharacter The hero character that will own the spawned weapons
	 * @param InCombatComponent The combat component where weapons will be registered
	 * @param ApplyLevel The weapon level/power (currently unused but available for stat scaling)
	 * 
	 * @see RegisterSpawnedWeapon for weapon registration
	 */
	void GiveWeaponsToCombatComponent(ACharacter* InOwnerCharacter, class UHeroCombatComponent* InCombatComponent, int32 ApplyLevel = 1);

private:

	/** 
	 * Array of abilities to grant when this hero is initialized.
	 * Each entry contains an ability class and the input tag to trigger it.
	 * TitleProperty = "InputTag" makes the UI display the InputTag as the item name.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData", meta = (TitleProperty = "InputTag"))
	TArray<FWarriorHeroAbilitySet> HeroStartUpAbilitySets;

	/**
	 * Array of weapons to spawn when this hero is initialized.
	 * Each entry specifies a weapon class, gameplay tag, and configuration data.
	 * TitleProperty = "WeaponTag" makes the UI display the WeaponTag as the item name.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData", meta = (TitleProperty = "WeaponTag"))
	TArray<FWarriorHeroStartUpWeapon> HeroStartUpWeapons;

};
