// pravin's patashala All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "UAbilityTask_WaitEnemyBoxTrace.generated.h"

class AWarriorHeroCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnEnemyBoxTraceHitSignature,
    AWarriorHeroCharacter*,
    HitHero
);

UCLASS()
class ADVANCEDRPG_API UUAbilityTask_WaitEnemyBoxTrace : public UAbilityTask
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnEnemyBoxTraceHitSignature OnBoxTraceHit;

    UFUNCTION(BlueprintCallable,
        Category = "Warrior|AbilityTasks",
        meta = (DisplayName = "Enemy Box Trace",
            HidePin = "OwningAbility",
            DefaultToSelf = "OwningAbility",
            BlueprintInternalUseOnly = "TRUE"))
    static UUAbilityTask_WaitEnemyBoxTrace* EnemyBoxTrace(
        UGameplayAbility* OwningAbility,
        FVector BoxTraceStart,
        FVector BoxTraceEnd,
        FVector BoxHalfSize,
        FRotator BoxOrientation,
        FGameplayEffectSpecHandle EffectSpecHandle
    );

    virtual void Activate() override;

private:
    void PerformBoxTrace();

    FVector CachedStart;
    FVector CachedEnd;
    FVector CachedBoxHalfSize;
    FRotator CachedBoxOrientation;
    FGameplayEffectSpecHandle CachedEffectSpecHandle;
};