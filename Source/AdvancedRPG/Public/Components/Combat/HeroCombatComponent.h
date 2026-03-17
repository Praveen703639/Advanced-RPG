// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "HeroCombatComponent.generated.h"

class AWarriorHeroWeapon;

/**
 * UHeroCombatComponent
 * 
 * Hero-specific combat component that extends the base pawn combat component.
 * Provides hero-specific weapon management with type-safe casting to AWarriorHeroWeapon.
 * 
 * Design Pattern: Type Specialization
 * - Inherits weapon storage from UPawnCombatComponent
 * - Adds hero-specific weapon accessor that returns properly typed weapons
 * - Allows Blueprint and code to get weapons without manual casting
 * 
 * Usage:
 * Call GetHeroCarriedWeaponByTag to get weapons with proper AWarriorHeroWeapon type.
 * 
 * @see UPawnCombatComponent for base weapon management
 */
UCLASS()
class ADVANCEDRPG_API UHeroCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()

public:

	/**
	 * Gets a hero weapon by its gameplay tag.
	 * Retrieves a carried weapon and casts it to AWarriorHeroWeapon for type safety.
	 * 
	 * This provides a convenient type-safe accessor for hero-specific weapons.
	 * Returns nullptr if the weapon is not found or is not a AWarriorHeroWeapon.
	 * 
	 * @param InWeaponTagToGive The gameplay tag identifying the weapon (e.g., "Weapon.Axe")
	 * @return The weapon as AWarriorHeroWeapon if found, nullptr otherwise
	 * 
	 * @see UPawnCombatComponent::GetCharacterCarriedWeaponByTag for base implementation
	 */
	UFUNCTION(BlueprintPure, Category = "Warrior|Combat")
	AWarriorHeroWeapon* GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTagToGive) const;
};
