// pravin's patashala all Rights Reserved

#include "AbilitySystem/AbilityTasks/UAT_SpawnIceClone.h"
#include "AbilitySystemComponent.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Controllers/WarriorAIController.h"
#include "GameplayTagContainer.h"
#include "Characters/AWarriorIceClone.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

UUAT_SpawnIceClone* UUAT_SpawnIceClone::SpawnIceClone(
    UGameplayAbility* OwningAbility,
    FGameplayTag EventTag,
    TSubclassOf<AAWarriorIceClone> CloneClass,
    float CloneDuration,
    float LeapDistance,
    bool bShouldLeapOnEvent,
    UParticleSystem* InIceParticleEffect,
    UParticleSystem* InParticleEffect,
    FName InParticleAttachSocket)
{
    UUAT_SpawnIceClone* Task = NewAbilityTask<UUAT_SpawnIceClone>(OwningAbility);
    Task->ListenEventTag = EventTag;
    Task->IceCloneClass = CloneClass;
    Task->Duration = CloneDuration;
    Task->LeapDist = LeapDistance;
    Task->bLeapOnEvent = bShouldLeapOnEvent;
    Task->IceParticleEffect = InIceParticleEffect;
    Task->ParticleEffect = InParticleEffect;
    Task->ParticleAttachSocket = InParticleAttachSocket;
    return Task;
}

void UUAT_SpawnIceClone::Activate()
{
    Super::Activate();

    // ✅ FIX: AbilitySystemComponent is raw pointer, use == nullptr
    if (AbilitySystemComponent == nullptr)
    {
        EndTask();
        return;
    }

    if (AActor* Avatar = Ability->GetAvatarActorFromActorInfo())
    {
        CachedOriginalLocation = Avatar->GetActorLocation();
        CachedOriginalRotation = Avatar->GetActorRotation();
    }

    FGameplayTagContainer TagContainer(ListenEventTag);
    FGameplayEventTagMulticastDelegate::FDelegate EventDelegate;
    EventDelegate.BindUObject(this, &UUAT_SpawnIceClone::OnGameplayEventReceived);

    EventDelegateHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(TagContainer, EventDelegate);

    SetWaitingOnAvatar();
}

// ✅ FIX: Delegate signature uses const FGameplayEventData* (pointer!)
void UUAT_SpawnIceClone::OnGameplayEventReceived(const FGameplayTag CallbackTag, const FGameplayEventData* Payload)
{
    if (CallbackTag != ListenEventTag)
    {
        return;
    }

    SpawnTheClone();

    if (bLeapOnEvent)
    {
        PerformHeroLeap();
    }

    GetWorld()->GetTimerManager().SetTimer(
        CloneDurationTimerHandle,
        this,
        &UUAT_SpawnIceClone::DestroySpawnedClone,
        Duration,
        false
    );

    OnCloneSpawned.Broadcast(SpawnedClone.Get());
}

void UUAT_SpawnIceClone::SpawnTheClone()
{
    if (!IceCloneClass)
    {
        return;
    }

    AActor* Avatar = Ability->GetAvatarActorFromActorInfo();
    if (!Avatar)
    {
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Avatar;
    SpawnParams.Instigator = Cast<APawn>(Avatar);
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    SpawnedClone = GetWorld()->SpawnActor<AAWarriorIceClone>(
        IceCloneClass,
        CachedOriginalLocation,
        CachedOriginalRotation,
        SpawnParams
    );

    if (!SpawnedClone.IsValid())
    {
        return;
    }

    SpawnedClone->InitializeClone(Cast<ACharacter>(Avatar), Duration);

    // Cascade particle at clone location
    if (IceParticleEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            IceParticleEffect,
            CachedOriginalLocation,
            CachedOriginalRotation,
            true
        );
    }

    // Cascade particle attached to hero
    if (ParticleEffect && Avatar)
    {
        AttachedParticleComponent = UGameplayStatics::SpawnEmitterAttached(
            ParticleEffect,
            Avatar->GetRootComponent(),
            ParticleAttachSocket,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::SnapToTarget,
            true
        );
    }

    // Force AI to target clone
    TArray<AActor*> AllEnemies;
    UGameplayStatics::GetAllActorsOfClass(this, AWarriorEnemyCharacter::StaticClass(), AllEnemies);

    for (AActor* Enemy : AllEnemies)
    {
        APawn* EnemyPawn = Cast<APawn>(Enemy);
        if (!EnemyPawn) continue;

        AController* EnemyController = EnemyPawn->GetController();
        if (!EnemyController) continue;

        AWarriorAIController* AIController = Cast<AWarriorAIController>(EnemyController);
        if (!AIController) continue;

        if (UBlackboardComponent* BB = AIController->GetBlackboardComponent())
        {
            AActor* CurrentTarget = Cast<AActor>(BB->GetValueAsObject(FName("TargetActor")));
            if (CurrentTarget == Avatar)
            {
                AIController->ForceTargetClone(SpawnedClone.Get());
            }
        }
    }
}

void UUAT_SpawnIceClone::PerformHeroLeap()
{
    AActor* Avatar = Ability->GetAvatarActorFromActorInfo();
    AWarriorHeroCharacter* Hero = Cast<AWarriorHeroCharacter>(Avatar);
    if (!Hero)
    {
        return;
    }

    FVector MoveDir = Hero->GetVelocity();
    if (MoveDir.IsNearlyZero())
    {
        MoveDir = Hero->GetActorForwardVector();
    }
    else
    {
        MoveDir.Z = 0.f;
        MoveDir.Normalize();
    }

    FVector Destination = CachedOriginalLocation + (MoveDir * LeapDist);

    FHitResult Hit;
    FVector TraceStart = Destination + FVector(0.f, 0.f, 100.f);
    FVector TraceEnd = Destination - FVector(0.f, 0.f, 1000.f);

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Hero);

    if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params))
    {
        Destination = Hit.ImpactPoint + FVector(0.f, 0.f, 10.f);
    }

    Hero->SetActorLocation(Destination, true, nullptr, ETeleportType::TeleportPhysics);
}

void UUAT_SpawnIceClone::DestroySpawnedClone()
{
    if (bIsBeingDestroyed) return;  // 🛡️ Already destroying
    bIsBeingDestroyed = true;

    if (AttachedParticleComponent.IsValid())
    {
        AttachedParticleComponent->DeactivateSystem();
        AttachedParticleComponent = nullptr;
    }

    if (SpawnedClone.IsValid())
    {
        TArray<AActor*> AllEnemies;
        UGameplayStatics::GetAllActorsOfClass(this, AWarriorEnemyCharacter::StaticClass(), AllEnemies);

        for (AActor* Enemy : AllEnemies)
        {
            APawn* EnemyPawn = Cast<APawn>(Enemy);
            if (!EnemyPawn) continue;

            AController* EnemyController = EnemyPawn->GetController();
            if (!EnemyController) continue;

            AWarriorAIController* AIController = Cast<AWarriorAIController>(EnemyController);
            if (AIController)
            {
                AIController->ClearCloneTarget();
            }
        }

        SpawnedClone->DestroyClone();
        SpawnedClone = nullptr;
    }

    OnCloneDestroyed.Broadcast();
    EndTask();

   
}

void UUAT_SpawnIceClone::ExternalCancel()
{
    if (bIsBeingDestroyed) return;  // 🛡️

    DestroySpawnedClone();
    Super::ExternalCancel();
}
void UUAT_SpawnIceClone::OnDestroy(bool AbilityEnded)
{
    // 🛡️ GUARD: Prevent re-entrant destruction
    if (bIsBeingDestroyed)
    {
        return;
    }
    bIsBeingDestroyed = true;

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(CloneDurationTimerHandle);
    }

    if (AttachedParticleComponent.IsValid())
    {
        AttachedParticleComponent->DeactivateSystem();
        AttachedParticleComponent = nullptr;
    }

    // ❌ REMOVE THIS — OnDestroy should NOT call EndTask-path code
    // DestroySpawnedClone();  

    // ✅ Only do cleanup that DOESN'T call EndTask
    if (SpawnedClone.IsValid())
    {
        SpawnedClone->DestroyClone();
        SpawnedClone = nullptr;
    }

    // ✅ Let parent finish — this is the ONLY place EndTask's cleanup runs
    Super::OnDestroy(AbilityEnded);
}