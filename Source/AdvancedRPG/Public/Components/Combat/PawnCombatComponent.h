// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "GameplayTagContainer.h"

#include "PawnCombatComponent.generated.h"

class AWarriorWeaponBase;

/**
 * UPawnCombatComponent
 * 
 * Base combat component for pawn characters that manages weapon storage and equipping.
 * Allows characters to carry multiple weapons and track which one is currently equipped.
 * 
 * Design Pattern: Weapon Management System
 * - Characters can carry multiple weapons (stored in CharacterCarriedWeaponMap)
 * - Each weapon is identified by a gameplay tag (e.g., "Weapon.Axe", "Weapon.Sword")
 * - Only one weapon can be equipped at a time (tracked by CurrentEquippedWeaponTag)
 * - Derived classes can override to add hero-specific or enemy-specific behavior
 * 
 * Usage:
 * 1. Weapon registers itself when spawned (RegisterSpawnedWeapon)
 * 2. Character equips weapon by tag (updates CurrentEquippedWeaponTag)
 * 3. Get current equipped weapon with GetCharacterCurrentEquippedWeapon
 * 4. Get any carried weapon by tag with GetCharacterCarriedWeaponByTag
 * 
 * @see UHeroCombatComponent for hero-specific weapon handling
 */
UCLASS()
class ADVANCEDRPG_API UPawnCombatComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

public:

	/**
	 * Registers a spawned weapon with the combat component.
	 * Called when a weapon is created to add it to the character's inventory.
	 * 
	 * @param InWeaponTagToRegister The gameplay tag identifying this weapon (e.g., "Weapon.Axe")
	 * @param InWeaponToRegister The weapon actor to register
	 * @param bRegisterAsEquippedWeapon If true, this weapon becomes the currently equipped weapon
	 */
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon = false);

	/**
	 * Gets a carried weapon by its gameplay tag.
	 * Searches the weapon map for a weapon matching the provided tag.
	 * 
	 * @param InWeaponTagToGet The gameplay tag to search for
	 * @return The weapon if found, nullptr otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;

	/**
	 * Gets the currently equipped weapon.
	 * Looks up the weapon using CurrentEquippedWeaponTag.
	 * 
	 * @return The currently equipped weapon, or nullptr if none is equipped
	 */
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCurrentEquippedWeapon() const;

protected:

	/** 
	 * The gameplay tag of the currently equipped weapon.
	 * Used to quickly identify which weapon is active without searching the map.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Warrior|Combat")
	FGameplayTag CurrentEquippedWeaponTag;


private:

	/** 
	 * Map of all weapons carried by this character.
	 * Key: FGameplayTag (weapon identifier like "Weapon.Axe")
	 * Value: AWarriorWeaponBase* (the weapon actor)
	 * 
	 * Allows fast O(1) lookup of weapons by tag instead of searching arrays.
	 */
	TMap<FGameplayTag, AWarriorWeaponBase*> CharacterCarriedWeaponMap;
};

