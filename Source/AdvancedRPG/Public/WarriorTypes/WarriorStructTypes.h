// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"


#include "WarriorStructTypes.generated.h"
class UWarriorAnimLinkedLayer;
class UWarriorGameplayAbility;
class UInputMappingContext;
class AActor;

/**
 * FWarriorHeroAbilitySet
 * 
 * Represents a single ability that can be granted to a hero character.
 * This structure ties together a gameplay ability with an input tag for easy input binding.
 * 
 * Used by: DataAsset_HeroStartUpData to grant starting abilities, weapon systems for ability bindings
 * 
 * Example: A fireball ability with input tag "Ability.Magic.Fireball" allows the ability system
 * to automatically bind this ability to the correct input action from the input mapping context.
 */
USTRUCT(BlueprintType)
struct FWarriorHeroAbilitySet
{
	GENERATED_BODY()

public:
	/** Gameplay tag that identifies this ability input. Used for input binding and ability activation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InputTag")
	FGameplayTag InputTag;

	/** The ability class to grant to the character. Must be a valid ability class or the struct will fail validation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UWarriorGameplayAbility> AbilityToGrant;

	/**
	 * Validates that this ability set has all required data.
	 * 
	 * @return true if both InputTag and AbilityToGrant are valid, false otherwise
	 * 
	 * Safety check used before granting abilities to prevent invalid ability configurations.
	 */
	bool IsValid() const;

};

/**
 * FWarriorHeroWeaponData
 * 
 * Defines the complete configuration for a weapon, including animations, input mappings, and abilities.
 * This structure is the core data container that represents a fully-equipped weapon state.
 * 
 * Design Pattern: Data-Driven Weapon System
 * - Weapons are defined as data, not code, allowing designers to create new weapons without programming
 * - Animation layers are linked for seamless weapon-specific animations
 * - Input contexts are dynamically applied when a weapon is equipped
 * - Abilities define weapon-specific moves (combo attacks, special abilities, etc.)
 * 
 * Usage Flow:
 * 1. Designer creates weapon data asset and fills in WeaponAnimLayerToLink
 * 2. When hero equips weapon, WeaponInputMappingContext is applied to input subsystem (priority 1)
 * 3. Combat component links WeaponAnimLayerToLink for animation overrides
 * 4. Ability system grants all DefaultHeroWeaponAbilitys to the character
 * 
 * Example: Axe Weapon
 * - WeaponAnimLayerToLink: BP_Axe_AnimLayer (overrides weapon montages)
 * - WeaponInputMappingContext: IMC_Axe (maps axe-specific combos to inputs)
 * - DefaultHeroWeaponAbilitys: [GA_Axe_LightAttack, GA_Axe_HeavyAttack, GA_Axe_Spin]
 */
USTRUCT(BlueprintType)
struct FWarriorHeroWeaponData
{
	GENERATED_BODY()

	/** Animation layer to apply to the character skeleton when this weapon is equipped. Handles weapon-specific animations and montages. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UWarriorAnimLinkedLayer> WeaponAnimLayerToLink;

	/** Enhanced input mapping context that defines this weapon's input actions. Applied with priority 1 when weapon is equipped, removed when unequipped. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputMappingContext* WeaponInputMappingContext = nullptr;

	/** Array of abilities granted when this weapon is equipped. Includes basic attacks, combos, and special abilities. Array allows flexible weapon designs. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta =  (TitleProperty = "InputTag"))
	TArray<FWarriorHeroAbilitySet> DefaultHeroWeaponAbilitys;


};

/**
 * FWarriorHeroStartUpWeapon
 * 
 * Defines a weapon that should be spawned and registered when a hero character is initialized.
 * This structure ties together the weapon class with its gameplay tag and configuration data.
 * 
 * Design Pattern: Data-Driven Weapon Initialization
 * - Weapons are spawned during character initialization rather than on-demand
 * - Each weapon has a unique gameplay tag for identification and lookup
 * - Configuration data (animations, abilities) is bound to the weapon through HeroWeaponData
 * 
 * Usage:
 * Add instances of this struct to a hero startup data asset to define which weapons spawn at startup.
 * Example: Tag="Weapon.Axe", WeaponClass=BP_HeroAxe, then GA_Hero_EquipAxe can find and use it.
 * 
 * @see FWarriorHeroWeaponData for weapon configuration
 */
USTRUCT(BlueprintType)
struct FWarriorHeroStartUpWeapon
{
	GENERATED_BODY()

public:
	/** Gameplay tag that uniquely identifies this weapon (e.g., "Weapon.Axe", "Weapon.Sword"). Used by abilities to request and equip weapons. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Tag")
	FGameplayTag WeaponTag;

	/** The weapon actor class to spawn. Use AActor to avoid compile-time dependency on concrete weapon class. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Class")
	TSubclassOf<AActor> WeaponClass;

	/** Configuration data applied to this weapon when it is spawned. Includes animations, input bindings, and abilities. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Data")
	FWarriorHeroWeaponData HeroWeaponData;

	/**
	 * Validates that this weapon startup configuration has all required data.
	 * 
	 * @return true if WeaponTag and WeaponClass are valid, false otherwise
	 * 
	 * Safety check used before spawning to prevent invalid weapon configurations.
	 */
	bool IsValid() const;
};
