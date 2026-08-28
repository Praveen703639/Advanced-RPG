// pravin's patashala all Rights Reserved 

#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Items/Weapons/WarriorHeroWeapon.h"
#include "GameFramework/Character.h"
#include "WarriorDebugHelper.h"

// ------------------------------------------------------------------
// Helper: Grants a single ability set to the ASC
// Works for both basic and special ability sets since FWarriorHeroSpeacialAbilitySet 
// inherits from FWarriorHeroAbilitySet
// ------------------------------------------------------------------
static void GrantAbilitySetToASC(UWarriorAbilitySystemComponent* InASCToGive, const FWarriorHeroAbilitySet& AbilitySet, int32 ApplyLevel)
{
    // Validate the ability set before attempting to grant it
    if (!AbilitySet.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("DataAsset_HeroStartUpData: AbilitySet invalid (InputTag=%s)"), *AbilitySet.InputTag.ToString());
        return;
    }

    // Double-check ability class is valid
    if (!AbilitySet.AbilityToGrant)
    {
        UE_LOG(LogTemp, Warning, TEXT("DataAsset_HeroStartUpData: AbilityToGrant is null for tag %s"), *AbilitySet.InputTag.ToString());
        return;
    }

    // Get the default object (recommended for UE5.6)
    UGameplayAbility* DefaultObj = AbilitySet.AbilityToGrant.GetDefaultObject();
    if (!DefaultObj)
    {
        UE_LOG(LogTemp, Warning, TEXT("DataAsset_HeroStartUpData: Failed to get default object for ability class for tag %s"), *AbilitySet.InputTag.ToString());
        return;
    }

    // Build the ability spec
    FGameplayAbilitySpec AbilitySpec(DefaultObj, ApplyLevel);
    AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
    AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);

    // Grant and log
    FGameplayAbilitySpecHandle Handle = InASCToGive->GiveAbility(AbilitySpec);
    UE_LOG(LogTemp, Log, TEXT("DataAsset_HeroStartUpData: Granted ability '%s' (Tag=%s) -> Handle valid: %s"),
        *AbilitySet.AbilityToGrant->GetName(),
        *AbilitySet.InputTag.ToString(),
        Handle.IsValid() ? TEXT("Yes") : TEXT("No"));
}

// ------------------------------------------------------------------
// Main: Grant all startup abilities (basic + special)
// ------------------------------------------------------------------
void UDataAsset_HeroStartUpData::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
    // Call parent implementation for base setup
    Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

    // --- Grant Basic Abilities ---
    for (const FWarriorHeroAbilitySet& AbilitySet : HeroStartUpAbilitySets)
    {
        GrantAbilitySetToASC(InASCToGive, AbilitySet, ApplyLevel);
    }

    // --- Grant Special Abilities ---
    // FWarriorHeroSpeacialAbilitySet inherits from FWarriorHeroAbilitySet, 
    // so we can pass it directly to the same grant function
    for (const FWarriorHeroSpeacialAbilitySet& SpecialAbilitySet : HeroSpecialAbilitySets)
    {
        GrantAbilitySetToASC(InASCToGive, SpecialAbilitySet, ApplyLevel);
    }
}

// ------------------------------------------------------------------
// Weapons (unchanged logic, cleaned up debug messages)
// ------------------------------------------------------------------
void UDataAsset_HeroStartUpData::GiveWeaponsToCombatComponent(ACharacter* InOwnerCharacter, UHeroCombatComponent* InCombatComponent, int32 ApplyLevel)
{
    check(InOwnerCharacter);
    check(InCombatComponent);

    for (const FWarriorHeroStartUpWeapon& StartUpWeapon : HeroStartUpWeapons)
    {
        // Validate weapon config
        if (!StartUpWeapon.IsValid())
        {
            UE_LOG(LogTemp, Warning, TEXT("DataAsset_HeroStartUpData: Weapon configuration invalid, skipping..."));
            continue;
        }

        // Get weapon class
        UClass* WeaponUClass = StartUpWeapon.WeaponClass.Get();
        if (!WeaponUClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("DataAsset_HeroStartUpData: Weapon class is null for tag: %s"), *StartUpWeapon.WeaponTag.ToString());
            continue;
        }

        // Spawn parameters
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = InOwnerCharacter;
        SpawnParams.Instigator = InOwnerCharacter->GetInstigator();
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        // Spawn weapon
        AWarriorHeroWeapon* SpawnedWeapon = InOwnerCharacter->GetWorld()->SpawnActor<AWarriorHeroWeapon>(
            WeaponUClass,
            InOwnerCharacter->GetActorLocation(),
            InOwnerCharacter->GetActorRotation(),
            SpawnParams
        );

        if (!SpawnedWeapon)
        {
            UE_LOG(LogTemp, Warning, TEXT("DataAsset_HeroStartUpData: Failed to spawn weapon for tag: %s"), *StartUpWeapon.WeaponTag.ToString());
            continue;
        }

        // Apply data and register
        SpawnedWeapon->HeroWeaponData = StartUpWeapon.HeroWeaponData;
        InCombatComponent->RegisterSpawnedWeapon(StartUpWeapon.WeaponTag, SpawnedWeapon);

        UE_LOG(LogTemp, Log, TEXT("DataAsset_HeroStartUpData: Weapon '%s' spawned and registered with tag '%s'"),
            *SpawnedWeapon->GetName(),
            *StartUpWeapon.WeaponTag.ToString());
    }
}