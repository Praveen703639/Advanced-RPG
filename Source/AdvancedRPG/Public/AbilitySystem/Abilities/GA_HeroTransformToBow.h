// pravin's patashala all Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GA_HeroTransformToBow.generated.h"

// Forward declarations
class AWarriorWeaponBase;

/**
 * Gameplay Ability that transforms the Hero character into the Bow character.
 * Inherits transform configuration from UWarriorGameplayAbility.
 */
UCLASS()
class ADVANCEDRPG_API UGA_HeroTransformToBow : public UWarriorHeroGameplayAbility
{
    GENERATED_BODY()

public:
    UGA_HeroTransformToBow();

protected:
    virtual bool CanActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayTagContainer* SourceTags = nullptr,
        const FGameplayTagContainer* TargetTags = nullptr,
        FGameplayTagContainer* OptionalRelevantTags = nullptr
    ) const override;

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;

    /** Called when transformation completes successfully */
    UFUNCTION()
    void HandleTransformComplete(AActor* NewCharacter);

    /** Called when transformation fails */
    UFUNCTION()
    void HandleTransformFailed();



private:
    /** Captured weapon to transfer to bow character */
    UPROPERTY()
    TObjectPtr<AWarriorWeaponBase> CapturedEquippedWeapon;

    /** Tag of the captured weapon */
    UPROPERTY()
    FGameplayTag CapturedEquippedWeaponTag;
};