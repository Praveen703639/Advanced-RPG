// pravin's  patashala all Rights Reserved 

#include "AbilitySystem/AbilityTasks/AbilityTask_WaitSpawnBossEnemies.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/AssetManager.h"
#include "NavigationSystem.h"
#include "Characters/WarriorEnemyCharacter.h"

#include "WarriorDebugHelper.h"



UAbilityTask_WaitSpawnBossEnemies* UAbilityTask_WaitSpawnBossEnemies::WaitSpawnBossEnemies(
    UGameplayAbility* OwningAbility,
    FGameplayTag EventTag,
    const TArray<TSoftClassPtr<AWarriorEnemyCharacter>>& ClassesToSpawn,
    int32 NumToSpawn,
    const FVector& SpawnOrigin,
    float RandomSpawnRadius)
{
    UAbilityTask_WaitSpawnBossEnemies* MyObj = NewAbilityTask<UAbilityTask_WaitSpawnBossEnemies>(OwningAbility);
    MyObj->CachedClassesToSpawn = ClassesToSpawn;
    MyObj->CachedNumToSpawn = NumToSpawn;
    MyObj->CachedSpawnOrigin = SpawnOrigin;
    MyObj->CachedRandomSpawnRadius = RandomSpawnRadius;
	MyObj->CachedEventTag = EventTag;

    return MyObj;
}

void UAbilityTask_WaitSpawnBossEnemies::Activate()
{
    FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);
    DelegateHandle = Delegate.AddUObject(this, &ThisClass::OnGameplayEventReceived);

}

void UAbilityTask_WaitSpawnBossEnemies::OnDestroy(bool bInOwnerFinished)
{
    FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);
    Delegate.Remove(DelegateHandle);

	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitSpawnBossEnemies::OnGameplayEventReceived(const FGameplayEventData* InPayload)
{
    if (CachedClassesToSpawn.IsEmpty())
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
        }

        EndTask();
        return;
    }

    TArray<FSoftObjectPath> Paths;
    for (const TSoftClassPtr<AWarriorEnemyCharacter>& SoftClass : CachedClassesToSpawn)
    {
        if (!SoftClass.IsNull())
        {
            Paths.Add(SoftClass.ToSoftObjectPath());
        }
    }

    if (Paths.IsEmpty())
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
        }

        EndTask();
        return;
    }

    UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
        Paths,
        FStreamableDelegate::CreateUObject(this, &ThisClass::OnEnemyClassesLoaded)
    );
}


void UAbilityTask_WaitSpawnBossEnemies::OnEnemyClassesLoaded()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
        }

        EndTask();
        return;
    }

    // Collect loaded UClass pointers
    TArray<UClass*> LoadedClasses;
    for (const TSoftClassPtr<AWarriorEnemyCharacter>& SoftClass : CachedClassesToSpawn)
    {
        UClass* Cls = SoftClass.Get();
        if (Cls)
        {
            LoadedClasses.Add(Cls);
        }
    }

    if (LoadedClasses.IsEmpty())
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
        }
        EndTask();
        return;
    }

    // Spawn one enemy for each loaded class (spawn all elements at once)
    TArray<AWarriorEnemyCharacter*> SpawnedEnemies;
    FActorSpawnParameters SpawnParam;
    SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    for (UClass* ChosenClass : LoadedClasses)
    {
        if (!ChosenClass)
        {
            continue;
        }

        FVector RandomLocation;
        UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this, CachedSpawnOrigin, RandomLocation, CachedRandomSpawnRadius);
        RandomLocation += FVector(0.f, 0.f, 150.f);

        const FRotator SpawnFacingRotation = AbilitySystemComponent->GetAvatarActor()->GetActorForwardVector().ToOrientationRotator();

        AWarriorEnemyCharacter* SpawnedEnemy = World->SpawnActor<AWarriorEnemyCharacter>(ChosenClass, RandomLocation, SpawnFacingRotation, SpawnParam);

        if (SpawnedEnemy)
        {
            SpawnedEnemies.Add(SpawnedEnemy);
        }
    }

    if (ShouldBroadcastAbilityTaskDelegates())
    {
        if (!SpawnedEnemies.IsEmpty())
        {
            OnSpawnFinished.Broadcast(SpawnedEnemies);
        }
        else
        {
            DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
        }
    }

    EndTask();
}

