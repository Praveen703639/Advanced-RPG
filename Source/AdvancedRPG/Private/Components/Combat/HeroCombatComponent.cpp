// pravin's  patashala all Rights Reserved 


#include "Components/Combat/HeroCombatComponent.h"
#include "Items/Weapons/WarriorHeroWeapon.h"


AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTagToGive) const
{
    // Get the weapon from the parent class (cast to correct type)
    // This provides type-safe access to hero weapons
    return Cast<AWarriorHeroWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTagToGive));
}
