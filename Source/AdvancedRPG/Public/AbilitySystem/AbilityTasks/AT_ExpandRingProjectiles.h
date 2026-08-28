#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameplayEffectTypes.h"
#include "AT_ExpandRingProjectiles.generated.h"

class AWarriorProjectileBase;
class UNiagaraSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExpandRingProjectilesComplete);

UCLASS()
class ADVANCEDRPG_API UAT_ExpandRingProjectiles : public UAbilityTask
{
    GENERATED_BODY()

public:
    UAT_ExpandRingProjectiles();

    UPROPERTY(BlueprintAssignable)
    FOnExpandRingProjectilesComplete OnRingComplete;

    UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Ability|Tasks")
    static UAT_ExpandRingProjectiles* ExpandRingProjectiles(
        UGameplayAbility* OwningAbility,
        TSubclassOf<AWarriorProjectileBase> InProjectileClass,
        FGameplayEffectSpecHandle InDamageEffectSpecHandle,
        UNiagaraSystem* InNiagaraEffect,
        int32 InNumProjectiles = 10,
        float InExpansionSpeed = 800.0f,
        float InSpawnRadius = 100.0f,
        float InStaggerDelay = 0.0f,
        float InProjectileLifeSpan = 4.0f,
        float InZOffset = 50.0f);

    virtual void Activate() override;
    virtual void ExternalCancel() override;
    virtual void OnDestroy(bool AbilityEnded) override;

protected:
    UPROPERTY()
    TSubclassOf<AWarriorProjectileBase> ProjectileClass;

    UPROPERTY()
    TArray<AWarriorProjectileBase*> SpawnedProjectiles;

    UPROPERTY()
    UNiagaraSystem* NiagaraEffect;

    FGameplayEffectSpecHandle DamageEffectSpecHandle;

    int32 NumProjectiles;
    float ExpansionSpeed;
    float SpawnRadius;
    float StaggerDelay;
    float ProjectileLifeSpan;
    float ZOffset;
    int32 CurrentSpawnIndex;

    FTimerHandle SpawnTimer;
    FTimerHandle CompleteTimer;

    void StartRingSpawn();
    void SpawnProjectileBatch();
    void SpawnNextProjectile();
    void SetupProjectile(AWarriorProjectileBase* Proj, const FVector& Direction);
    void CleanupAndEnd();
};