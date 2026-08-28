// pravin's patashala all Rights Reserved

#include "AbilitySystem/AbilityTasks/AbilityTask_WaitSpawnEnemies.h"

#include "AbilitySystemComponent.h"
#include "Engine/AssetManager.h"
#include "NavigationSystem.h"

UAbilityTask_WaitSpawnEnemies*
UAbilityTask_WaitSpawnEnemies::WaitSpawnEnemies(
    UGameplayAbility* OwningAbility,
    FGameplayTag EventTag,
    TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn,
    int32 NumToSpawn,
    FVector SpawnOrigin,
    float RandomSpawnRadius,
    EEnemySpawnType SpawnType
)
{
    UAbilityTask_WaitSpawnEnemies* Node =
        NewAbilityTask<UAbilityTask_WaitSpawnEnemies>(OwningAbility);

    Node->CachedEventTag = EventTag;
    Node->CachedSoftEnemyClassToSpawn = SoftEnemyClassToSpawn;
    Node->CachedNumToSpawn = NumToSpawn;
    Node->CachedSpawnOrigin = SpawnOrigin;
    Node->CachedRandomSpawnRadius = RandomSpawnRadius;
    Node->CachedSpawnType = SpawnType;

    return Node;
}

void UAbilityTask_WaitSpawnEnemies::Activate()
{
    FGameplayEventMulticastDelegate& Delegate =
        AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(
            CachedEventTag
        );

    DelegateHandle =
        Delegate.AddUObject(
            this,
            &ThisClass::OnGameplayEventReceived
        );
}

void UAbilityTask_WaitSpawnEnemies::OnDestroy(bool bInOwnerFinished)
{
    FGameplayEventMulticastDelegate& Delegate =
        AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(
            CachedEventTag
        );

    Delegate.Remove(DelegateHandle);

    Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitSpawnEnemies::OnGameplayEventReceived(
    const FGameplayEventData* InPayload
)
{
    if (ensure(!CachedSoftEnemyClassToSpawn.IsNull()))
    {
        UAssetManager::Get()
            .GetStreamableManager()
            .RequestAsyncLoad(
                CachedSoftEnemyClassToSpawn.ToSoftObjectPath(),
                FStreamableDelegate::CreateUObject(
                    this,
                    &ThisClass::OnEnemyClassLoaded
                )
            );
    }
    else
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            DidNotSpawn.Broadcast(
                TArray<AWarriorEnemyCharacter*>()
            );
        }

        EndTask();
    }
}

void UAbilityTask_WaitSpawnEnemies::OnEnemyClassLoaded()
{
    UClass* LoadedClass =
        CachedSoftEnemyClassToSpawn.Get();

    UWorld* World = GetWorld();

    if (!LoadedClass || !World)
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            DidNotSpawn.Broadcast(
                TArray<AWarriorEnemyCharacter*>()
            );
        }

        EndTask();
        return;
    }

    TArray<AWarriorEnemyCharacter*> SpawnedEnemies;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    for (int32 i = 0; i < CachedNumToSpawn; i++)
    {
        FVector SpawnLocation = CachedSpawnOrigin;

        if (CachedSpawnType == EEnemySpawnType::Ground)
        {
            bool bFoundLocation =
                UNavigationSystemV1::K2_GetRandomReachablePointInRadius(
                    this,
                    CachedSpawnOrigin,
                    SpawnLocation,
                    CachedRandomSpawnRadius
                );

            if (!bFoundLocation)
            {
                continue;
            }

            SpawnLocation += FVector(0.f, 0.f, 150.f);
        }
        else
        {
            FVector2D RandomOffset =
                FMath::RandPointInCircle(
                    CachedRandomSpawnRadius
                );

            SpawnLocation += FVector(
                RandomOffset.X,
                RandomOffset.Y,
                FMath::FRandRange(-200.f, 200.f)
            );
        }

        const FRotator SpawnRotation =
            AbilitySystemComponent
            ->GetAvatarActor()
            ->GetActorForwardVector()
            .ToOrientationRotator();

        AWarriorEnemyCharacter* SpawnedEnemy =
            World->SpawnActor<AWarriorEnemyCharacter>(
                LoadedClass,
                SpawnLocation,
                SpawnRotation,
                SpawnParams
            );

        if (SpawnedEnemy)
        {
            SpawnedEnemies.Add(SpawnedEnemy);
        }
    }

    if (ShouldBroadcastAbilityTaskDelegates())
    {
        if (!SpawnedEnemies.IsEmpty())
        {
            OnSpawnFinished.Broadcast(
                SpawnedEnemies
            );
        }
        else
        {
            DidNotSpawn.Broadcast(
                TArray<AWarriorEnemyCharacter*>()
            );
        }
    }

    EndTask();
}