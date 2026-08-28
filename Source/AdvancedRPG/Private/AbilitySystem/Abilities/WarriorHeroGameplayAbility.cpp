// Pravin's Pathshala - All Rights Reserved

#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "GameplayEffect.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Controllers/WarriorHeroController.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "warriorGameplayTags.h"
#include "Items/Weapons/WarriorWeaponBase.h"
#include "Characters/WarriorBaseCharacter.h"


AWarriorHeroCharacter* UWarriorHeroGameplayAbility::GetWarriorHeroCharacterFromActorInfo()
{
    // Return cached if still valid
    if (CachedWarriorHeroCharacter.IsValid())
    {
        return CachedWarriorHeroCharacter.Get();
    }

    // Safety: CurrentActorInfo can be null during ability cleanup
    if (!CurrentActorInfo)
    {
        return nullptr;
    }

    // Cast the avatar actor (character) from CurrentActorInfo
    CachedWarriorHeroCharacter = Cast<AWarriorHeroCharacter>(CurrentActorInfo->AvatarActor);

    return CachedWarriorHeroCharacter.IsValid() ? CachedWarriorHeroCharacter.Get() : nullptr;
}

AWarriorHeroController* UWarriorHeroGameplayAbility::GetWarriorHeroControllerFromActorInfo()
{
    // Return cached if still valid
    if (CachedWarriorHeroController.IsValid())
    {
        return CachedWarriorHeroController.Get();
    }

    // Safety: CurrentActorInfo can be null during ability cleanup
    if (!CurrentActorInfo)
    {
        return nullptr;
    }

    // Try 1: Direct PlayerController from ActorInfo (works on client & listen server host)
    if (CurrentActorInfo->PlayerController.IsValid())
    {
        CachedWarriorHeroController = Cast<AWarriorHeroController>(CurrentActorInfo->PlayerController);
    }

    // Try 2: Get controller from the character itself (works on dedicated server!)
    // This is the CRITICAL fallback dude — PlayerController isn't always cached in ActorInfo on server
    if (!CachedWarriorHeroController.IsValid())
    {
        if (AWarriorHeroCharacter* HeroChar = GetWarriorHeroCharacterFromActorInfo())
        {
            CachedWarriorHeroController = Cast<AWarriorHeroController>(HeroChar->GetController());
        }
    }

    return CachedWarriorHeroController.IsValid() ? CachedWarriorHeroController.Get() : nullptr;
}

UHeroCombatComponent* UWarriorHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
    // Get the hero character with null check
    AWarriorHeroCharacter* HeroCharacter = GetWarriorHeroCharacterFromActorInfo();

    // Only attempt to get the combat component if the character is valid
    if (HeroCharacter)
    {
        return HeroCharacter->GetHeroCombatComponent();
    }

    // Return nullptr if character was not available
    return nullptr;
}

FGameplayEffectSpecHandle UWarriorHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, int32 InUsedComboCount)
{
    check(EffectClass);

    FGameplayEffectContextHandle ContextHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeEffectContext();
    ContextHandle.SetAbility(this);
    ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
    ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

    FGameplayEffectSpecHandle EffectSpecHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
        EffectClass,
        GetAbilityLevel(),
        ContextHandle
    );

    EffectSpecHandle.Data->SetSetByCallerMagnitude(
        WarriorGameplayTags::Shared_SetByCaller_BaseDamage,
        InWeaponBaseDamage
    );

    if (InCurrentAttackTypeTag.IsValid())
    {
        EffectSpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag, InUsedComboCount);
    }

    return EffectSpecHandle;
}

bool UWarriorHeroGameplayAbility::GetAbilityRemainingCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldownTime, float& RemainingCooldownTime)
{
    check(InCooldownTag.IsValid());

    FGameplayEffectQuery CooldownQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InCooldownTag.GetSingleTagContainer());

    TArray< TPair <float, float> > TimeRemainingAndDuration = GetAbilitySystemComponentFromActorInfo()->GetActiveEffectsTimeRemainingAndDuration(CooldownQuery);

    if (!TimeRemainingAndDuration.IsEmpty())
    {
        RemainingCooldownTime = TimeRemainingAndDuration[0].Key;
        TotalCooldownTime = TimeRemainingAndDuration[0].Value;
    }

    return RemainingCooldownTime > 0.f;
}

float UWarriorHeroGameplayAbility::GetCooldownDurationFromGE() const
{
    // GetCooldownGameplayEffect() returns a direct UGameplayEffect* instance pointer in modern versions of GAS
    const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
    if (!CooldownGE)
    {
        return 0.f;
    }

    // Read the ScalableFloat duration value at the current ability level
    float OutMagnitude = 0.f;
    const FGameplayEffectModifierMagnitude& DurationMag = CooldownGE->DurationMagnitude;

    // GetStaticMagnitudeIfPossible takes (float InLevel, float& OutMagnitude) and returns a bool success state
    bool bSuccess = DurationMag.GetStaticMagnitudeIfPossible(GetAbilityLevel(), OutMagnitude);

    return bSuccess ? OutMagnitude : 0.f;
}

UHeroUIComponent* UWarriorHeroGameplayAbility::GetHeroUIComponentFromActorInfo()
{
    return GetWarriorHeroCharacterFromActorInfo() ? GetWarriorHeroCharacterFromActorInfo()->GetHeroUIComponent() : nullptr;
}

