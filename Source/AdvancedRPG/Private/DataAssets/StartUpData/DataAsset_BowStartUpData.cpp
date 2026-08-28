// pravin's  patashala all Rights Reserved 

#include "DataAssets/StartUpData/DataAsset_BowStartUpData.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "Components/Combat/BowCombatComponent.h"
#include "Items/Weapons/WarriorArrow.h"
#include "GameFramework/Character.h"

void UDataAsset_BowStartUpData::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
    // Call parent for base behavior (effects, other arrays)
    Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

    if (!InASCToGive) return;

    for (const FWarriorHeroAbilitySet& AbilitySet : BowStartUpAbilitySets)
    {
        if (!AbilitySet.IsValid()) continue;

        if (!AbilitySet.AbilityToGrant)
        {
            UE_LOG(LogTemp, Warning, TEXT("DataAsset_BowStartUpData: AbilityToGrant is null for tag %s"), *AbilitySet.InputTag.ToString());
            continue;
        }

        UGameplayAbility* DefaultObj = AbilitySet.AbilityToGrant.GetDefaultObject();
        if (!DefaultObj)
        {
            UE_LOG(LogTemp, Warning, TEXT("DataAsset_BowStartUpData: Failed to get default object for ability class for tag %s"), *AbilitySet.InputTag.ToString());
            continue;
        }

        FGameplayAbilitySpec AbilitySpec(DefaultObj, ApplyLevel);
        AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
        AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);

        FGameplayAbilitySpecHandle Handle = InASCToGive->GiveAbility(AbilitySpec);
        UE_LOG(LogTemp, Warning, TEXT("DataAsset_BowStartUpData: Granted ability '%s' (Tag=%s) -> Handle valid: %s"),
            *AbilitySet.AbilityToGrant->GetName(), *AbilitySet.InputTag.ToString(), Handle.IsValid() ? TEXT("Yes") : TEXT("No"));
    }

    // Spawn/register weapons if any (reusing hero startup weapon struct)
    if (BowStartUpWeapons.Num() > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("DataAsset_BowStartUpData: BowStartUpWeapons present but automatic spawn requires character-specific call."));
    }
}

void UDataAsset_BowStartUpData::GiveBowWeaponsToCombatComponent(ACharacter* InOwnerCharacter, UBowCombatComponent* InCombatComponent, int32 ApplyLevel)
{
    // Fix 1: Spawn and register bow weapons (arrows) so shooting abilities can find them.
    check(InOwnerCharacter);
    check(InCombatComponent);

    for (const FWarriorHeroStartUpWeapon& StartUpWeapon : BowStartUpWeapons)
    {
        if (!StartUpWeapon.IsValid())
        {
            UE_LOG(LogTemp, Warning, TEXT("DataAsset_BowStartUpData: Invalid weapon config, skipping."));
        continue;
    }

        UClass* WeaponUClass = StartUpWeapon.WeaponClass.Get();
        if (!WeaponUClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("DataAsset_BowStartUpData: WeaponClass is null for tag: %s"), *StartUpWeapon.WeaponTag.ToString());
            continue;
        }

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = InOwnerCharacter;
        SpawnParams.Instigator = InOwnerCharacter->GetInstigator();
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        // Spawn as AWarriorArrow (subclass of AWarriorWeaponBase)
        AWarriorArrow* SpawnedArrow = InOwnerCharacter->GetWorld()->SpawnActor<AWarriorArrow>(
            WeaponUClass,
            InOwnerCharacter->GetActorLocation(),
            InOwnerCharacter->GetActorRotation(),
            SpawnParams
        );

        if (!SpawnedArrow)
        {
            UE_LOG(LogTemp, Warning, TEXT("DataAsset_BowStartUpData: Failed to spawn arrow for tag: %s"), *StartUpWeapon.WeaponTag.ToString());
            continue;
        }

        // Register the arrow with the bow combat component
        InCombatComponent->RegisterSpawnedWeapon(StartUpWeapon.WeaponTag, SpawnedArrow, true /* set as equipped */);

        UE_LOG(LogTemp, Warning, TEXT("DataAsset_BowStartUpData: Arrow '%s' spawned and registered with tag '%s'"),
            *SpawnedArrow->GetName(), *StartUpWeapon.WeaponTag.ToString());
    }
}
