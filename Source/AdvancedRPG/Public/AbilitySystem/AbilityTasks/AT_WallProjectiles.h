// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameplayEffectTypes.h"
#include "AT_WallProjectiles.generated.h"

class AWarriorProjectileBase;
class UNiagaraSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallProjectilesComplete);

UCLASS()
class ADVANCEDRPG_API UAT_WallProjectiles : public UAbilityTask
{
    GENERATED_BODY()

public:
    UAT_WallProjectiles();

    UPROPERTY(BlueprintAssignable)
    FOnWallProjectilesComplete OnWallComplete;

    UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Ability|Tasks")
    static UAT_WallProjectiles* WallProjectiles(
        UGameplayAbility* OwningAbility,
        TSubclassOf<AWarriorProjectileBase> InProjectileClass,
        FGameplayEffectSpecHandle InDamageEffectSpecHandle,
        UNiagaraSystem* InNiagaraEffect,
        FTransform InSpawnTransform,
        int32 InNumProjectiles = 8,
        float InProjectileSpacing = 120.0f,
        float InSpeed = 1600.0f,
        float InSpawnDistance = 0.0f,
        float InProjectileLifeSpan = 3.0f,
        float InZOffset = 0.0f);

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
    float ProjectileSpacing;
    float Speed;
    float SpawnDistance;
    float ProjectileLifeSpan;
    float ZOffset;

    FTimerHandle CompleteTimer;

    void SpawnWall();
    void SetupProjectile(AWarriorProjectileBase* Proj, const FVector& Direction);
    void CleanupAndEnd();
};



