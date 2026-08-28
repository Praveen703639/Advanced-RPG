// Pravin's Pathshala - All Rights Reserved
#include "Components/Combat/PawnCombatComponent.h"
#include "Items/Weapons/WarriorWeaponBase.h"
#include "Components/CapsuleComponent.h"

#include "WarriorDebugHelper.h"

UPawnCombatComponent::UPawnCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPawnCombatComponent::RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon)
{
	// SAFETY: Don't crash if weapon spawn failed (enemy startup data can pass null)
	if (!InWeaponToRegister)
	{
		UE_LOG(LogTemp, Warning, TEXT("RegisterSpawnedWeapon: Weapon is NULL for tag %s, skipping registration"), *InWeaponTagToRegister.ToString());
		return;
	}

	// Ensure the weapon tag is unique - prevent duplicate weapon registrations
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister), TEXT("A Tag named %s has already been added to weapon registry! Duplicate weapon registration prevented."), *InWeaponTagToRegister.ToString());

	// Add the weapon to the map using the provided tag
	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister, InWeaponToRegister);

	InWeaponToRegister->OnWeaponHitTarget.BindUObject(this, &UPawnCombatComponent::OnHitTargetActor);
	InWeaponToRegister->OnWeaponPulledFromTarget.BindUObject(this, &UPawnCombatComponent::OnWeaponPulledFromTargetActor);

	// If this weapon should be equipped by default, set it as the current equipped weapon
	if (bRegisterAsEquippedWeapon)
	{
		CurrentEquippedWeaponTag = InWeaponTagToRegister;
	}

	// NEW: Call hook so derived classes (Hero) can rebind to their own hit functions
	PostRegisterWeapon(InWeaponToRegister);
}

void UPawnCombatComponent::PostRegisterWeapon(AWarriorWeaponBase* InWeaponToRegister)
{
	// Base does nothing — HeroCombatComponent overrides this
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
			UE_LOG(LogTemp, Warning, TEXT("Weapon found: %s"), *FoundWeapon[0]->GetName());
			return *FoundWeapon;
		}
	}

	return nullptr;
}

AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
	// Return nullptr if no weapon is currently equipped
	if (!CurrentEquippedWeaponTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("GetCharacterCurrentEquippedWeapon: CurrentEquippedWeaponTag is INVALID!"));
		return nullptr;
	}

	// Get the weapon using the currently equipped weapon tag
	AWarriorWeaponBase* EquippedWeapon = GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);

	if (!EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetCharacterCurrentEquippedWeapon: Weapon not found in map for tag: %s"),
			*CurrentEquippedWeaponTag.ToString());
	}

	return EquippedWeapon;
}

void UPawnCombatComponent::TogggleWeaponCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	if (ToggleDamageType == EToggleDamageType::CurrentEquippedWeapon)
	{
		ToggleCurrentEquippedWeaponCollision(bShouldEnable);
	}
	else
	{
		ToggleBodyCollisionBoxCollision(bShouldEnable, ToggleDamageType);
	}
}

void UPawnCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (HitActor)
	{
		OverlappedActors.AddUnique(HitActor);
	}
}

void UPawnCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
}

void UPawnCombatComponent::ToggleCurrentEquippedWeaponCollision(bool bShouldEnable)
{
	AWarriorWeaponBase* WeaponToToggle = GetCharacterCurrentEquippedWeapon();
	check(WeaponToToggle);

	if (bShouldEnable)
	{
		// Clear hit memory BEFORE enabling so each new swing starts fresh.
		OverlappedActors.Empty();
		WeaponToToggle->GetWeaponCollisionCapsule()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		WeaponToToggle->GetWeaponCollisionCapsule()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OverlappedActors.Empty();
	}
}

void UPawnCombatComponent::ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
}

void UPawnCombatComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Dude, clone dies = equipped weapon dies with it. Clean and simple!
	if (AWarriorWeaponBase* EquippedWeapon = GetCharacterCurrentEquippedWeapon())
	{
		if (IsValid(EquippedWeapon))
		{
			UE_LOG(LogTemp, Log, TEXT("PawnCombatComponent: Destroying equipped weapon %s with owner %s"),
				*EquippedWeapon->GetName(), *GetNameSafe(GetOwner()));

			EquippedWeapon->Destroy();
		}
	}

	// Always call super at the end!
	Super::EndPlay(EndPlayReason);
}