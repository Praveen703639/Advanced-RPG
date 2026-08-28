#include "AbilitySystem/AbilityTasks/AT_ExpandRingProjectiles.h"
#include "Items/Projectiles/WarriorProjectileBase.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemComponent.h"

UAT_ExpandRingProjectiles::UAT_ExpandRingProjectiles()
    : NumProjectiles(10)
    , ExpansionSpeed(800.0f)
    , SpawnRadius(100.0f)
    , StaggerDelay(0.0f)
    , ProjectileLifeSpan(4.0f)
    , ZOffset(50.0f)
    , CurrentSpawnIndex(0)
{
}

UAT_ExpandRingProjectiles* UAT_ExpandRingProjectiles::ExpandRingProjectiles(
    UGameplayAbility* OwningAbility,
    TSubclassOf<AWarriorProjectileBase> InProjectileClass,
    FGameplayEffectSpecHandle InDamageEffectSpecHandle,
    UNiagaraSystem* InNiagaraEffect,
    int32 InNumProjectiles,
    float InExpansionSpeed,
    float InSpawnRadius,
    float InStaggerDelay,
    float InProjectileLifeSpan,
    float InZOffset)
{
    UAT_ExpandRingProjectiles* Task = NewAbilityTask<UAT_ExpandRingProjectiles>(OwningAbility);
    Task->ProjectileClass = InProjectileClass;
    Task->DamageEffectSpecHandle = InDamageEffectSpecHandle;
    Task->NiagaraEffect = InNiagaraEffect;
    Task->NumProjectiles = FMath::Clamp(InNumProjectiles, 1, 24);
    Task->ExpansionSpeed = InExpansionSpeed;
    Task->SpawnRadius = InSpawnRadius;
    Task->StaggerDelay = InStaggerDelay;
    Task->ProjectileLifeSpan = InProjectileLifeSpan;
    Task->ZOffset = InZOffset;
    return Task;
}

void UAT_ExpandRingProjectiles::Activate()
{
    Super::Activate();
    StartRingSpawn();
}

void UAT_ExpandRingProjectiles::StartRingSpawn()
{
    if (!Ability || !Ability->GetCurrentActorInfo() || !Ability->GetCurrentActorInfo()->AvatarActor.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("ExpandRingProjectiles: Invalid Ability or Avatar!"));
        EndTask();
        return;
    }

    if (!ProjectileClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ExpandRingProjectiles: ProjectileClass is NULL!"));
        EndTask();
        return;
    }

    CurrentSpawnIndex = 0;

    if (StaggerDelay > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            SpawnTimer,
            this,
            &UAT_ExpandRingProjectiles::SpawnNextProjectile,
            StaggerDelay,
            true);
    }
    else
    {
        SpawnProjectileBatch();
    }

    // Auto-cleanup after all projectiles should be dead
    float EndDelay = ProjectileLifeSpan + 1.0f;
    if (StaggerDelay > 0.0f)
    {
        EndDelay += (NumProjectiles * StaggerDelay);
    }

    GetWorld()->GetTimerManager().SetTimer(
        CompleteTimer,
        this,
        &UAT_ExpandRingProjectiles::CleanupAndEnd,
        EndDelay,
        false);
}

void UAT_ExpandRingProjectiles::SpawnProjectileBatch()
{
    AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
    if (!Avatar) return;

    FVector Center = Avatar->GetActorLocation();

    for (int32 i = 0; i < NumProjectiles; i++)
    {
        float AngleDeg = (360.0f / NumProjectiles) * i;
        float AngleRad = FMath::DegreesToRadians(AngleDeg);
        FVector Dir = FVector(FMath::Cos(AngleRad), FMath::Sin(AngleRad), 0.0f).GetSafeNormal();
        FVector SpawnLoc = Center + (Dir * SpawnRadius) + FVector(0.0f, 0.0f, ZOffset);
        FTransform SpawnTransform(Dir.Rotation(), SpawnLoc);

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        Params.Owner = Avatar;
        Params.Instigator = Cast<APawn>(Avatar);

        // DEFERRED SPAWN — collision not active yet
        AWarriorProjectileBase* Proj = GetWorld()->SpawnActorDeferred<AWarriorProjectileBase>(
            ProjectileClass, SpawnTransform, Avatar, Cast<APawn>(Avatar), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

        if (Proj)
        {
            // Configure BEFORE collision becomes active
            SetupProjectile(Proj, Dir);
            Proj->FinishSpawning(SpawnTransform);
            SpawnedProjectiles.Add(Proj);
        }
    }
}

void UAT_ExpandRingProjectiles::SpawnNextProjectile()
{
    AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
    if (!Avatar || CurrentSpawnIndex >= NumProjectiles)
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
        return;
    }

    FVector Center = Avatar->GetActorLocation();
    float AngleDeg = (360.0f / NumProjectiles) * CurrentSpawnIndex;
    float AngleRad = FMath::DegreesToRadians(AngleDeg);
    FVector Dir = FVector(FMath::Cos(AngleRad), FMath::Sin(AngleRad), 0.0f).GetSafeNormal();
    FVector SpawnLoc = Center + (Dir * SpawnRadius) + FVector(0.0f, 0.0f, ZOffset);
    FTransform SpawnTransform(Dir.Rotation(), SpawnLoc);

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    Params.Owner = Avatar;
    Params.Instigator = Cast<APawn>(Avatar);

    // DEFERRED SPAWN
    AWarriorProjectileBase* Proj = GetWorld()->SpawnActorDeferred<AWarriorProjectileBase>(
        ProjectileClass, SpawnTransform, Avatar, Cast<APawn>(Avatar), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

    if (Proj)
    {
        SetupProjectile(Proj, Dir);
        Proj->FinishSpawning(SpawnTransform);
        SpawnedProjectiles.Add(Proj);
    }

    CurrentSpawnIndex++;
    if (CurrentSpawnIndex >= NumProjectiles)
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
    }
}

void UAT_ExpandRingProjectiles::SetupProjectile(AWarriorProjectileBase* Proj, const FVector& Direction)
{
    if (!Proj) return;

    FVector Velocity = Direction * ExpansionSpeed;

    // Use public setters — no more protected access errors
    Proj->SetProjectileVelocity(Velocity);
    Proj->SetProjectileSpeed(ExpansionSpeed, ExpansionSpeed * 1.2f);
    Proj->SetProjectileGravityScale(0.0f); // Lady of the Pond — zero gravity fire orbs

    // Pass GAS damage spec handle
    Proj->SetDamageEffectSpecHandle(DamageEffectSpecHandle);

    // Set and activate Niagara effect
    if (NiagaraEffect)
    {
        Proj->SetNiagaraEffect(NiagaraEffect);
        Proj->ActivateNiagaraComponent(true);
    }

    // Face direction of travel
    Proj->SetActorRotation(Direction.Rotation());

    // Lifespan so they don't float forever
    Proj->SetLifeSpan(ProjectileLifeSpan);
}

void UAT_ExpandRingProjectiles::CleanupAndEnd()
{
    GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
    GetWorld()->GetTimerManager().ClearTimer(CompleteTimer);

    for (AWarriorProjectileBase* Proj : SpawnedProjectiles)
    {
        if (IsValid(Proj))
        {
            Proj->Destroy();
        }
    }
    SpawnedProjectiles.Empty();

    OnRingComplete.Broadcast();
    EndTask();
}

void UAT_ExpandRingProjectiles::ExternalCancel()
{
    CleanupAndEnd();
    Super::ExternalCancel();
}

void UAT_ExpandRingProjectiles::OnDestroy(bool AbilityEnded)
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
        GetWorld()->GetTimerManager().ClearTimer(CompleteTimer);
    }
    Super::OnDestroy(AbilityEnded);
}