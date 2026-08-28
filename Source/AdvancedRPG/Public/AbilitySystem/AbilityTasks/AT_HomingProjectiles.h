#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameplayEffectTypes.h"
#include "AT_HomingProjectiles.generated.h"

class AWarriorProjectileBase;
class UNiagaraSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHomingProjectilesComplete);

UCLASS()
class ADVANCEDRPG_API UAT_HomingProjectiles : public UAbilityTask
{
    GENERATED_BODY()

public:
    UAT_HomingProjectiles();

    UPROPERTY(BlueprintAssignable)
    FOnHomingProjectilesComplete OnHomingComplete;

    UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Ability|Tasks")
    static UAT_HomingProjectiles* HomingProjectiles(
        UGameplayAbility* OwningAbility,
        TSubclassOf<AWarriorProjectileBase> InProjectileClass,
        FGameplayEffectSpecHandle InDamageEffectSpecHandle,
        UNiagaraSystem* InNiagaraEffect,
        FTransform InSpawnTransform,
        int32 InNumProjectiles = 3,
        float InSpeed = 1200.0f,
        float InHomingAcceleration = 2500.0f,
        float InArcHeight = 400.0f,
        float InProjectileLifeSpan = 5.0f,
        float InStaggerDelay = 0.15f);

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
    FTransform SpawnTransform;

    int32 NumProjectiles;
    float Speed;
    float HomingAcceleration;
    float ArcHeight;
    float ProjectileLifeSpan;
    float StaggerDelay;
    int32 CurrentSpawnIndex;

    FTimerHandle SpawnTimer;
    FTimerHandle CompleteTimer;

    void StartHomingSpawn();
    void SpawnProjectileBatch();
    void SpawnNextProjectile();
    void SetupProjectile(AWarriorProjectileBase* Proj, const FVector& BaseDirection);
    void CleanupAndEnd();

    USceneComponent* FindTargetMesh() const;
};