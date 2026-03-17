// pravin's  patashala all Rights Reserved 


#include "WarriorTypes/WarriorStructTypes.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"

bool FWarriorHeroAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrant;
}

bool FWarriorHeroStartUpWeapon::IsValid() const
{
	return WeaponTag.IsValid() && WeaponClass;
}
