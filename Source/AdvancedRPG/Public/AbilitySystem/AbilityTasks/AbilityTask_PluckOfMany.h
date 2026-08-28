#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "AbilityTask_PluckOfMany.generated.h"

class UParticleSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPluckOfManyCloneSpawned, ACharacter*, SpawnedClone);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPluckOfManyAllClonesExpired, int32, TotalSpawnedCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPluckOfManyFailed);

UCLASS()
class ADVANCEDRPG_API UAbilityTask_PluckOfMany : public UAbilityTask
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks|PluckOfMany", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
    static UAbilityTask_PluckOfMany* PluckOfMany(
        UGameplayAbility* OwningAbility,
        TSubclassOf<AWarriorEnemyCharacter> CloneClass,
        int32 CloneCount,
        float CloneLifetime,
        float SpawnRadius,
        float SpawnDelayPerClone,
        UParticleSystem* SpawnEffect,
        UParticleSystem* StrandPluckEffect,
        UParticleSystem* CloneDeathEffect);

    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks|PluckOfMany")
    void StartSpawningClones();

    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks|PluckOfMany")
    void ExpireAllClones();

    UPROPERTY(BlueprintAssignable)
    FPluckOfManyCloneSpawned OnCloneSpawned;

    UPROPERTY(BlueprintAssignable)
    FPluckOfManyAllClonesExpired OnAllClonesExpired;

    UPROPERTY(BlueprintAssignable)
    FPluckOfManyFailed OnPluckFailed;

protected:
    virtual void Activate() override;
    virtual void OnDestroy(bool bInOwnerFinished) override;

private:
    void SpawnNextClone();
    void FinishCloneSpawning();
    FVector CalculateSpawnLocation(int32 CloneIndex) const;
    void OnCloneLifetimeExpired(ACharacter* ExpiredClone);

    UPROPERTY()
    TSubclassOf<AWarriorEnemyCharacter> CachedCloneClass;

    UPROPERTY()
    TObjectPtr<UParticleSystem> CachedSpawnEffect;

    UPROPERTY()
    TObjectPtr<UParticleSystem> CachedStrandPluckEffect;

    UPROPERTY()
    TObjectPtr<UParticleSystem> CachedCloneDeathEffect;

    float CachedCloneLifetime = 5.0f;
    float CachedSpawnRadius = 300.0f;
    float CachedSpawnDelayPerClone = 0.1f;
    int32 CachedCloneCount = 3;

    TWeakObjectPtr<ACharacter> SourceCharacter;
    int32 CurrentSpawnIndex = 0;
    int32 TotalSpawnedCount = 0;
    bool bHasStartedSpawning = false;

    FTimerHandle SpawnTimerHandle;
    TArray<TWeakObjectPtr<ACharacter>> ActiveClones;
    TMap<TWeakObjectPtr<ACharacter>, FTimerHandle> CloneLifetimeTimers;
};