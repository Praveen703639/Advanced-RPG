// pravin's patashala all Rights Reserved

#include "AbilitySystem/Abilities/GA_HeroTransformToBow.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_TransformCharacter.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Characters/WarriorBowCharacter.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Components/Combat/BowCombatComponent.h"      // 🔥 FIXED: Added missing include
#include "Items/Weapons/WarriorWeaponBase.h"
#include "Items/Weapons/WarriorArrow.h"               // 🔥 FIXED: Added missing include

UGA_HeroTransformToBow::UGA_HeroTransformToBow()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    ActivationPolicy = EWarriorAbilityActivationPolicy::OnTriggered;
}

bool UGA_HeroTransformToBow::CanActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayTagContainer* SourceTags,
    const FGameplayTagContainer* TargetTags,
    FGameplayTagContainer* OptionalRelevantTags
) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        return false;
    }
    return ActorInfo && ActorInfo->AvatarActor.IsValid() && ActorInfo->AvatarActor->IsA<AWarriorHeroCharacter>();
}

void UGA_HeroTransformToBow::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData
)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    UE_LOG(LogTemp, Warning, TEXT("=== HERO TO BOW TRANSFORM ACTIVATED ==="));

    AActor* SourceChar = GetAvatarActorFromActorInfo();

    // Destroy ALL hero weapons — bow spawns its own arrow fresh
    if (AWarriorHeroCharacter* HeroChar = Cast<AWarriorHeroCharacter>(SourceChar))
    {
        if (UHeroCombatComponent* CombatComp = HeroChar->GetHeroCombatComponent())
        {
            for (auto& WeaponPair : CombatComp->GetCharacterCarriedWeaponMap())
            {
                if (AWarriorWeaponBase* Weapon = WeaponPair.Value)
                {
                    if (IsValid(Weapon))
                    {
                        Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
                        Weapon->Destroy();
                        UE_LOG(LogTemp, Warning, TEXT("GA_HeroTransformToBow: Destroyed hero weapon '%s'"), *Weapon->GetName());
                    }
                }
            }
        }
    }

    // Create transform task — DO NOT pass any weapon
    UAbilityTask_TransformCharacter* TransformTask =
        UAbilityTask_TransformCharacter::TransformCharacter(
            this,
            ETransformType::HeroToBow,
            HeroClass,
            BowClass,
            HeroToBow_OutMontage,
            HeroToBow_InMontage,
            DissolveEffect,
            FormEffect,
            TransformDuration,
            true,
            CinematicCameraClass
        );

    if (!TransformTask)
    {
        HandleTransformFailed();
        return;
    }

    TransformTask->OnTransformSucceeded.AddDynamic(this, &UGA_HeroTransformToBow::HandleTransformComplete);
    TransformTask->OnTransformFailed.AddDynamic(this, &UGA_HeroTransformToBow::HandleTransformFailed);
    TransformTask->ReadyForActivation();
}

void UGA_HeroTransformToBow::HandleTransformComplete(AActor* NewCharacter)
{
    UE_LOG(LogTemp, Warning, TEXT("=== TRANSFORM COMPLETE ==="));

    // DO NOT grant abilities here — Super::PossessedBy() already did it.
    // Granting again creates duplicate specs with same InputTag, breaking input.

    if (AWarriorBowCharacter* BowChar = Cast<AWarriorBowCharacter>(NewCharacter))
    {
        if (UBowCombatComponent* BowCombat = BowChar->GetBowCombatComponent())
        {
            if (AWarriorArrow* Arrow = BowCombat->GetBowCurrentEquippedArrow())
            {
                UE_LOG(LogTemp, Warning, TEXT("Bow arrow equipped: %s"), *Arrow->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Bow arrow NOT equipped — calling SpawnDefaultBowArrow"));
                BowChar->SpawnDefaultBowArrow();
            }
        }
    }

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_HeroTransformToBow::HandleTransformFailed()
{
    UE_LOG(LogTemp, Error, TEXT("=== TRANSFORM FAILED ==="));
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}