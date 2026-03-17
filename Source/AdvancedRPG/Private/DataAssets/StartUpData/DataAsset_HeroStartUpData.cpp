// pravin's  patashala all Rights Reserved 


#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Items/Weapons/WarriorHeroWeapon.h"
#include "GameFramework/Character.h"
#include "WarriorDebugHelper.h"



void UDataAsset_HeroStartUpData::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
    // Call parent implementation for base setup
    Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

    // Iterate through all ability sets in this data asset
    for (const FWarriorHeroAbilitySet& AbilitySet : HeroStartUpAbilitySets)
    {
        // Validate the ability set before attempting to grant it
        // This prevents granting incomplete or invalid ability configurations
        if (!AbilitySet.IsValid()) 
            continue;

        // Create a gameplay ability spec from the ability class
        FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);

        // Set the source object to the character that owns the ability system
        // This allows abilities to know which actor is executing them
        AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();

        // Set the ability level (determines ability power/stats)
        AbilitySpec.Level = ApplyLevel;

        // Add the input tag as a dynamic source tag
        // This allows the input system to trigger the ability correctly
        // Using GetDynamicSpecSourceTags() is the modern UE5 API approach
        AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);

        // Grant the ability to the ability system component
        InASCToGive->GiveAbility(AbilitySpec);
    }
}

void UDataAsset_HeroStartUpData::GiveWeaponsToCombatComponent(ACharacter* InOwnerCharacter, UHeroCombatComponent* InCombatComponent, int32 ApplyLevel)
{
    // Ensure the character is valid
    check(InOwnerCharacter);
    check(InCombatComponent);

    // Iterate through all weapon sets in this data asset
    for (const FWarriorHeroStartUpWeapon& StartUpWeapon : HeroStartUpWeapons)
    {
        // Validate the weapon configuration before attempting to spawn
        if (!StartUpWeapon.IsValid())
        {
            const FString DebugMessage = FString::Printf(TEXT("Weapon configuration is invalid. Skipping..."));
            Debug::Print(DebugMessage, FColor::Red);
            continue;
        }

        // Ensure WeaponClass is valid
        UClass* WeaponUClass = StartUpWeapon.WeaponClass.Get();
        if (!WeaponUClass)
        {
            const FString DebugMessage = FString::Printf(TEXT("Weapon class is null for tag: %s"), *StartUpWeapon.WeaponTag.ToString());
            Debug::Print(DebugMessage, FColor::Red);
            continue;
        }

        // Prepare spawn parameters
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = InOwnerCharacter;
        SpawnParams.Instigator = InOwnerCharacter->GetInstigator();
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        // Spawn the weapon actor in the world at the character's location
        AWarriorHeroWeapon* SpawnedWeapon = InOwnerCharacter->GetWorld()->SpawnActor<AWarriorHeroWeapon>(
            WeaponUClass,
            InOwnerCharacter->GetActorLocation(),
            InOwnerCharacter->GetActorRotation(),
            SpawnParams
        );

        // Ensure the weapon was successfully spawned
        if (!SpawnedWeapon)
        {
            const FString DebugMessage = FString::Printf(TEXT("Failed to spawn weapon for tag: %s"), *StartUpWeapon.WeaponTag.ToString());
            Debug::Print(DebugMessage, FColor::Red);
            continue;
        }

        // Assign the weapon data (animations, abilities, input mappings) to the spawned weapon
        SpawnedWeapon->HeroWeaponData = StartUpWeapon.HeroWeaponData;

        // Register the weapon with the combat component
        // This makes it available for the character to equip via GetHeroCarriedWeaponByTag
        InCombatComponent->RegisterSpawnedWeapon(StartUpWeapon.WeaponTag, SpawnedWeapon);

        const FString DebugMessage = FString::Printf(TEXT("Weapon '%s' spawned and registered with tag '%s'"), 
            *SpawnedWeapon->GetName(), 
            *StartUpWeapon.WeaponTag.ToString());
        Debug::Print(DebugMessage);
    }
}
