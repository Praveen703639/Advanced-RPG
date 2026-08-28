// pravin's patashala All Rights Reserved

#include "AbilitySystem/AbilityTasks/UAbilityTask_WaitEnemyBoxTrace.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "Characters/WarriorHeroCharacter.h"

UUAbilityTask_WaitEnemyBoxTrace* UUAbilityTask_WaitEnemyBoxTrace::EnemyBoxTrace(
    UGameplayAbility* OwningAbility,
    FVector BoxTraceStart,
    FVector BoxTraceEnd,
    FVector BoxHalfSize,
    FRotator BoxOrientation,
    FGameplayEffectSpecHandle EffectSpecHandle)
{
    UUAbilityTask_WaitEnemyBoxTrace* Task = NewAbilityTask<UUAbilityTask_WaitEnemyBoxTrace>(OwningAbility);

    Task->CachedStart = BoxTraceStart;
    Task->CachedEnd = BoxTraceEnd;
    Task->CachedBoxHalfSize = BoxHalfSize;
    Task->CachedBoxOrientation = BoxOrientation;
    Task->CachedEffectSpecHandle = EffectSpecHandle;

    return Task;
}

void UUAbilityTask_WaitEnemyBoxTrace::Activate()
{
    PerformBoxTrace();
}

void UUAbilityTask_WaitEnemyBoxTrace::PerformBoxTrace()
{
    AActor* AvatarActor = GetAvatarActor();
    if (!AvatarActor)
    {
        OnBoxTraceHit.Broadcast(nullptr);
        EndTask();
        return;
    }

    APawn* OwningPawn = Cast<APawn>(AvatarActor);
    if (!OwningPawn)
    {
        OnBoxTraceHit.Broadcast(nullptr);
        EndTask();
        return;
    }

    TArray<FHitResult> HitResults;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(AvatarActor);

    UKismetSystemLibrary::BoxTraceMulti(
        this,
        CachedStart,
        CachedEnd,
        CachedBoxHalfSize,
        CachedBoxOrientation,
        UEngineTypes::ConvertToTraceType(ECC_Pawn),
        false,
        ActorsToIgnore,
        EDrawDebugTrace::None,
        HitResults,
        true
    );

    AWarriorHeroCharacter* FirstValidHero = nullptr;

    for (const FHitResult& Hit : HitResults)
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor)
        {
            continue;
        }

        AWarriorHeroCharacter* HitHero = Cast<AWarriorHeroCharacter>(HitActor);
        if (!HitHero)
        {
            continue;
        }

        if (!UWarriorFunctionLibrary::IsTargetPawnHostile(OwningPawn, HitHero))
        {
            continue;
        }

        FirstValidHero = HitHero;

        // Apply gameplay effect HERE — inside C++, safe and controlled
        if (CachedEffectSpecHandle.IsValid())
        {
            if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(HitHero))
            {
                if (UAbilitySystemComponent* TargetASC = ASI->GetAbilitySystemComponent())
                {
                    AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(
                        *CachedEffectSpecHandle.Data.Get(),
                        TargetASC
                    );
                }
            }
        }

        break; // Only first valid hero
    }

    OnBoxTraceHit.Broadcast(FirstValidHero);
    EndTask();
}