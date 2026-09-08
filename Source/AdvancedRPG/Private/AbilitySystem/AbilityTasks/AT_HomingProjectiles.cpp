#include "AbilitySystem/AbilityTasks/AT_HomingProjectiles.h"
#include "Items/Projectiles/WarriorProjectileBase.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemComponent.h"
#include "Components/SceneComponent.h"

UAT_HomingProjectiles::UAT_HomingProjectiles()
    : NumProjectiles(3)
    , Speed(1200.0f)
    , HomingAcceleration(2500.0f)
    , ArcHeight(400.0f)
    , ProjectileLifeSpan(5.0f)
    , StaggerDelay(0.15f)
    , CurrentSpawnIndex(0)
{
}

UAT_HomingProjectiles* UAT_HomingProjectiles::HomingProjectiles(
    UGameplayAbility* OwningAbility,
    AActor* InTargetActor,
    TSubclassOf<AWarriorProjectileBase> InProjectileClass,
    FGameplayEffectSpecHandle InDamageEffectSpecHandle,
    UNiagaraSystem* InNiagaraEffect,
    FTransform InSpawnTransform,
    int32 InNumProjectiles,
    float InSpeed,
    float InHomingAcceleration,
    float InArcHeight,
    float InProjectileLifeSpan,
    float InStaggerDelay)
{
    UAT_HomingProjectiles* Task = NewAbilityTask<UAT_HomingProjectiles>(OwningAbility);
    Task->TargetActor = InTargetActor;
    Task->ProjectileClass = InProjectileClass;
    Task->DamageEffectSpecHandle = InDamageEffectSpecHandle;
    Task->NiagaraEffect = InNiagaraEffect;
    Task->SpawnTransform = InSpawnTransform;
    Task->NumProjectiles = FMath::Clamp(InNumProjectiles, 1, 12);
    Task->Speed = FMath::Max(0.0f, InSpeed);
    Task->HomingAcceleration = FMath::Max(0.0f, InHomingAcceleration);
    Task->ArcHeight = InArcHeight;
    Task->ProjectileLifeSpan = FMath::Max(0.1f, InProjectileLifeSpan);
    Task->StaggerDelay = FMath::Max(0.0f, InStaggerDelay);
    return Task;
}

void UAT_HomingProjectiles::Activate()
{
    Super::Activate();
    StartHomingSpawn();
}

void UAT_HomingProjectiles::StartHomingSpawn()
{
    if (!Ability)
    {
        UE_LOG(LogTemp, Error, TEXT("HomingProjectiles: Invalid owning ability."));
        EndTask();
        return;
    }

    const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
    AActor* Avatar = ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr;

    if (!Avatar || !GetWorld())
    {
        UE_LOG(LogTemp, Error, TEXT("HomingProjectiles: Invalid avatar or world."));
        EndTask();
        return;
    }

    if (!TargetActor || !IsValid(TargetActor))
    {
        UE_LOG(LogTemp, Error, TEXT("HomingProjectiles: Invalid target actor."));
        EndTask();
        return;
    }

    if (!ProjectileClass)
    {
        UE_LOG(LogTemp, Error, TEXT("HomingProjectiles: Invalid projectile class."));
        EndTask();
        return;
    }

    CurrentSpawnIndex = 0;

    if (StaggerDelay > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            SpawnTimer,
            this,
            &UAT_HomingProjectiles::SpawnNextProjectile,
            StaggerDelay,
            true);
    }
    else
    {
        SpawnProjectileBatch();
    }

    const float EndDelay = ProjectileLifeSpan + 1.0f +
        (StaggerDelay > 0.0f ? NumProjectiles * StaggerDelay : 0.0f);

    GetWorld()->GetTimerManager().SetTimer(
        CompleteTimer,
        this,
        &UAT_HomingProjectiles::CleanupAndEnd,
        EndDelay,
        false);
}

USceneComponent* UAT_HomingProjectiles::FindTargetComponent() const
{
    return IsValid(TargetActor) ? TargetActor->GetRootComponent() : nullptr;
}

void UAT_HomingProjectiles::SpawnProjectileBatch()
{
    if (!Ability || !GetWorld())
    {
        EndTask();
        return;
    }

    const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
    AActor* Avatar = ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr;
    USceneComponent* TargetComp = FindTargetComponent();

    if (!Avatar || !TargetComp)
    {
        UE_LOG(LogTemp, Error, TEXT("HomingProjectiles: Missing avatar or target component."));
        EndTask();
        return;
    }

    const FVector Center = SpawnTransform.Equals(FTransform::Identity)
        ? Avatar->GetActorLocation()
        : SpawnTransform.GetLocation();

    const FVector Forward = (TargetComp->GetComponentLocation() - Center).GetSafeNormal();

    if (Forward.IsNearlyZero())
    {
        UE_LOG(LogTemp, Warning, TEXT("HomingProjectiles: Target is at spawn location."));
        EndTask();
        return;
    }

    for (int32 i = 0; i < NumProjectiles; ++i)
    {
        const FVector SpawnLoc = Center + (Forward * 120.0f) + FVector(0.0f, 0.0f, 80.0f);
        const FTransform FinalSpawnTransform(Forward.Rotation(), SpawnLoc);

        AWarriorProjectileBase* Proj = GetWorld()->SpawnActorDeferred<AWarriorProjectileBase>(
            ProjectileClass,
            FinalSpawnTransform,
            Avatar,
            Cast<APawn>(Avatar),
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

        if (Proj)
        {
            SetupProjectile(Proj, Forward);
            Proj->FinishSpawning(FinalSpawnTransform);
            Proj->SetHomingTarget(TargetComp, HomingAcceleration);
            SpawnedProjectiles.Add(Proj);
        }
    }
}

void UAT_HomingProjectiles::SpawnNextProjectile()
{
    if (!Ability || !GetWorld())
    {
        return;
    }

    const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
    AActor* Avatar = ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr;
    USceneComponent* TargetComp = FindTargetComponent();

    if (!Avatar || !TargetComp || CurrentSpawnIndex >= NumProjectiles)
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
        return;
    }

    const FVector Center = SpawnTransform.Equals(FTransform::Identity)
        ? Avatar->GetActorLocation()
        : SpawnTransform.GetLocation();

    const FVector Forward = (TargetComp->GetComponentLocation() - Center).GetSafeNormal();
    if (Forward.IsNearlyZero())
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
        return;
    }

    const FVector SpawnLoc = Center + (Forward * 120.0f) + FVector(0.0f, 0.0f, 80.0f);
    const FTransform FinalSpawnTransform(Forward.Rotation(), SpawnLoc);

    AWarriorProjectileBase* Proj = GetWorld()->SpawnActorDeferred<AWarriorProjectileBase>(
        ProjectileClass,
        FinalSpawnTransform,
        Avatar,
        Cast<APawn>(Avatar),
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

    if (Proj)
    {
        SetupProjectile(Proj, Forward);
        Proj->FinishSpawning(FinalSpawnTransform);
        Proj->SetHomingTarget(TargetComp, HomingAcceleration);
        SpawnedProjectiles.Add(Proj);
    }

    ++CurrentSpawnIndex;
    if (CurrentSpawnIndex >= NumProjectiles)
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
    }
}

void UAT_HomingProjectiles::SetupProjectile(AWarriorProjectileBase* Proj, const FVector& BaseDirection)
{
    if (!Proj) return;

    const FVector InitialVelocity = (BaseDirection * Speed) + FVector(0.0f, 0.0f, ArcHeight);

    Proj->SetProjectileVelocity(InitialVelocity);
    Proj->SetProjectileSpeed(Speed, Speed * 1.5f);
    Proj->SetProjectileGravityScale(0.0f);
    Proj->SetDamageEffectSpecHandle(DamageEffectSpecHandle);

    if (NiagaraEffect)
    {
        Proj->SetNiagaraEffect(NiagaraEffect);
        Proj->ActivateNiagaraComponent(true);
    }

    Proj->SetActorRotation(BaseDirection.Rotation());
    Proj->SetLifeSpan(ProjectileLifeSpan);
}

void UAT_HomingProjectiles::CleanupAndEnd(bool bBroadcastComplete)
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
        GetWorld()->GetTimerManager().ClearTimer(CompleteTimer);
    }

    for (AWarriorProjectileBase* Proj : SpawnedProjectiles)
    {
        if (IsValid(Proj))
        {
            Proj->Destroy();
        }
    }
    SpawnedProjectiles.Empty();

    if (bBroadcastComplete)
    {
        OnHomingComplete.Broadcast();
    }

    EndTask();
}

void UAT_HomingProjectiles::ExternalCancel()
{
    CleanupAndEnd(false);
    Super::ExternalCancel();
}

void UAT_HomingProjectiles::OnDestroy(bool AbilityEnded)
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
        GetWorld()->GetTimerManager().ClearTimer(CompleteTimer);
    }

    SpawnedProjectiles.Empty();
    Super::OnDestroy(AbilityEnded);
}