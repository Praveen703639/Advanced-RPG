// pravin's  patashala all Rights Reserved 


#include "Components/Combat/PawnCombatComponent.h"
#include "Items/Weapons/WarriorWeaponBase.h"

#include "WarriorDebugHelper.h"

void UPawnCombatComponent::RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon)
{
	// Ensure the weapon tag is unique - prevent duplicate weapon registrations
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister), TEXT("A Tag named %s has already been added to weapon registry! Duplicate weapon registration prevented."), *InWeaponTagToRegister.ToString());

	// Ensure the weapon pointer is valid
	check(InWeaponToRegister);

	// Add the weapon to the map using the provided tag
	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister, InWeaponToRegister);

	// If this weapon should be equipped by default, set it as the current equipped weapon
	if (bRegisterAsEquippedWeapon)
	{
		CurrentEquippedWeaponTag = InWeaponTagToRegister;
	}

	// Log the successful registration for debugging purposes
	const FString DebugMessage = FString::Printf(TEXT("Weapon '%s' registered with tag '%s'"), 
		*InWeaponToRegister->GetName(), 
		*InWeaponTagToRegister.ToString());
	Debug::Print(DebugMessage);
}

AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const
{
	// Debug: Log the requested weapon tag
	UE_LOG(LogTemp, Warning, TEXT("Attempting to get weapon with tag: %s"), *InWeaponTagToGet.ToString());

	// Check if the weapon exists in the map
	if (CharacterCarriedWeaponMap.Contains(InWeaponTagToGet))
	{
		// Find and return the weapon pointer
		if (AWarriorWeaponBase* const* FoundWeapon = CharacterCarriedWeaponMap.Find(InWeaponTagToGet))
		{
			UE_LOG(LogTemp, Warning, TEXT("✓ Weapon found: %s"), *FoundWeapon[0]->GetName());
			return *FoundWeapon;
		}
	}

	// Return nullptr if weapon not found
	UE_LOG(LogTemp, Error, TEXT("✗ Weapon NOT found for tag: %s"), *InWeaponTagToGet.ToString());

	// Debug: Print all available weapons in the map
	UE_LOG(LogTemp, Warning, TEXT("Available weapons in map:"));
	for (const auto& WeaponPair : CharacterCarriedWeaponMap)
	{
		UE_LOG(LogTemp, Warning, TEXT("  - Tag: %s, Weapon: %s"), 
			*WeaponPair.Key.ToString(), 
			*WeaponPair.Value->GetName());
	}

	return nullptr;
}

AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
	// Return nullptr if no weapon is currently equipped
	if (!CurrentEquippedWeaponTag.IsValid())
	{
		return nullptr;
	}

	// Get the weapon using the currently equipped weapon tag
	return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
}
