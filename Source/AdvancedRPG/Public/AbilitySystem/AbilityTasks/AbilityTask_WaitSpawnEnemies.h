// pravin's patashala all Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameplayTagContainer.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "AbilityTask_WaitSpawnEnemies.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnSpawnEnemiesSignature,
    const TArray<AWarriorEnemyCharacter*>&,
    SpawnedEnemies
);

UENUM(BlueprintType)
enum class EEnemySpawnType : uint8
{
    Ground UMETA(DisplayName = "Ground"),
    Air UMETA(DisplayName = "Air")
};

UCLASS()
class ADVANCEDRPG_API UAbilityTask_WaitSpawnEnemies : public UAbilityTask
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintAssignable)
    FOnSpawnEnemiesSignature OnSpawnFinished;

    UPROPERTY(BlueprintAssignable)
    FOnSpawnEnemiesSignature DidNotSpawn;

    UFUNCTION(BlueprintCallable,
        Category = "Warrior|AbilityTasks",
        meta = (DisplayName = "Wait Gameplay Event And Spawn Enemies",
            HidePin = "OwningAbility",
            DefaultToSelf = "OwningAbility",
            BlueprintInternalUseOnly = "TRUE"))
    static UAbilityTask_WaitSpawnEnemies* WaitSpawnEnemies(
        UGameplayAbility* OwningAbility,
        FGameplayTag EventTag,
        TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn,
        int32 NumToSpawn,
        FVector SpawnOrigin,
        float RandomSpawnRadius,
        EEnemySpawnType SpawnType
    );

    virtual void Activate() override;
    virtual void OnDestroy(bool bInOwnerFinished) override;

private:

    void OnGameplayEventReceived(const FGameplayEventData* InPayload);
    void OnEnemyClassLoaded();

protected:

    FGameplayTag CachedEventTag;

    TSoftClassPtr<AWarriorEnemyCharacter> CachedSoftEnemyClassToSpawn;

    int32 CachedNumToSpawn = 1;

    FVector CachedSpawnOrigin = FVector::ZeroVector;

    float CachedRandomSpawnRadius = 500.f;

    EEnemySpawnType CachedSpawnType = EEnemySpawnType::Ground;

    FDelegateHandle DelegateHandle;
};