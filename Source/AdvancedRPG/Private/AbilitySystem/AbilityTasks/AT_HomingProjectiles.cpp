// pravin's patashala all Rights Reserved

#include "AbilitySystem/AbilityTasks/AT_HomingProjectiles.h"
#include "Items/Projectiles/WarriorProjectileBase.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"

// Target characters
#include "Characters/WarriorHeroCharacter.h"
#include "Characters/WarriorBowCharacter.h"

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
    Task->ProjectileClass = InProjectileClass;
    Task->DamageEffectSpecHandle = InDamageEffectSpecHandle;
    Task->NiagaraEffect = InNiagaraEffect;
    Task->SpawnTransform = InSpawnTransform;
    Task->NumProjectiles = FMath::Clamp(InNumProjectiles, 1, 12);
    Task->Speed = InSpeed;
    Task->HomingAcceleration = InHomingAcceleration;
    Task->ArcHeight = InArcHeight;
    Task->ProjectileLifeSpan = InProjectileLifeSpan;
    Task->StaggerDelay = InStaggerDelay;
    return Task;
}

void UAT_HomingProjectiles::Activate()
{
    Super::Activate();
    StartHomingSpawn();
}

void UAT_HomingProjectiles::StartHomingSpawn()
{
    if (!Ability || !Ability->GetCurrentActorInfo() || !Ability->GetCurrentActorInfo()->AvatarActor.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("HomingProjectiles: Invalid Ability or Avatar!"));
        EndTask();
        return;
    }

    if (!ProjectileClass)
    {
        UE_LOG(LogTemp, Error, TEXT("HomingProjectiles: ProjectileClass is NULL!"));
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

    float EndDelay = ProjectileLifeSpan + 1.0f;
    if (StaggerDelay > 0.0f)
    {
        EndDelay += (NumProjectiles * StaggerDelay);
    }

    GetWorld()->GetTimerManager().SetTimer(
        CompleteTimer,
        this,
        &UAT_HomingProjectiles::CleanupAndEnd,
        EndDelay,
        false);
}

USceneComponent* UAT_HomingProjectiles::FindTargetMesh() const
{
    AActor* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn)
    {
        UE_LOG(LogTemp, Error, TEXT("HomingProjectiles: GetPlayerPawn returned NULL! No target found."));
        return nullptr;
    }

    if (AWarriorHeroCharacter* Hero = Cast<AWarriorHeroCharacter>(PlayerPawn))
    {
        return Hero->GetRootComponent(); // Capsule = mid-body height
    }

    if (AWarriorBowCharacter* BowHero = Cast<AWarriorBowCharacter>(PlayerPawn))
    {
        return BowHero->GetRootComponent(); // Capsule = mid-body height
    }

    if (ACharacter* Char = Cast<ACharacter>(PlayerPawn))
    {
        return Char->GetRootComponent(); // <-- FIX: No red squiggle, same result
    }

    return PlayerPawn->GetRootComponent();
}

void UAT_HomingProjectiles::SpawnProjectileBatch()
{
    AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
    if (!Avatar) return;

    USceneComponent* TargetComp = FindTargetMesh();
    if (!TargetComp)
    {
        UE_LOG(LogTemp, Error, TEXT("HomingProjectiles: No target found! Aborting batch."));
        EndTask();
        return;
    }

    FVector Center;
    if (!SpawnTransform.Equals(FTransform::Identity))
    {
        Center = SpawnTransform.GetLocation();
    }
    else
    {
        Center = Avatar->GetActorLocation();
    }

    // FIX: Aim at the target, NOT wherever the enemy is facing!
    FVector TargetLocation = TargetComp->GetComponentLocation();
    FVector Forward = (TargetLocation - Center).GetSafeNormal();

    for (int32 i = 0; i < NumProjectiles; i++)
    {
        // Spawn slightly forward and up so it doesn't clip the enemy's own collision
        FVector SpawnLoc = Center + (Forward * 120.0f) + FVector(0.0f, 0.0f, 80.0f);
        FTransform FinalSpawnTransform(Forward.Rotation(), SpawnLoc);

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

            if (TargetComp)
            {
                Proj->SetHomingTarget(TargetComp, HomingAcceleration);
            }

            SpawnedProjectiles.Add(Proj);
        }
    }
}

void UAT_HomingProjectiles::SpawnNextProjectile()
{
    AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
    if (!Avatar || CurrentSpawnIndex >= NumProjectiles)
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
        return;
    }

    USceneComponent* TargetComp = FindTargetMesh();
    if (!TargetComp)
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
        return;
    }

    FVector Center;
    if (!SpawnTransform.Equals(FTransform::Identity))
    {
        Center = SpawnTransform.GetLocation();
    }
    else
    {
        Center = Avatar->GetActorLocation();
    }

    // FIX: Aim at target location
    FVector TargetLocation = TargetComp->GetComponentLocation();
    FVector Forward = (TargetLocation - Center).GetSafeNormal();

    FVector SpawnLoc = Center + (Forward * 120.0f) + FVector(0.0f, 0.0f, 80.0f);
    FTransform FinalSpawnTransform(Forward.Rotation(), SpawnLoc);

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

        if (TargetComp)
        {
            Proj->SetHomingTarget(TargetComp, HomingAcceleration);
        }

        SpawnedProjectiles.Add(Proj);
    }

    CurrentSpawnIndex++;
    if (CurrentSpawnIndex >= NumProjectiles)
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
    }
}

void UAT_HomingProjectiles::SetupProjectile(AWarriorProjectileBase* Proj, const FVector& BaseDirection)
{
    if (!Proj) return;

    // Curved stunning path: forward + arc height
    FVector InitialVelocity = (BaseDirection * Speed) + FVector(0.0f, 0.0f, ArcHeight);

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

    UE_LOG(LogTemp, Log, TEXT("HomingProjectiles: SetupProjectile complete. Vel=%s"), *InitialVelocity.ToString());
}

void UAT_HomingProjectiles::CleanupAndEnd()
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

    OnHomingComplete.Broadcast();
    EndTask();
}

void UAT_HomingProjectiles::ExternalCancel()
{
    CleanupAndEnd();
    Super::ExternalCancel();
}

void UAT_HomingProjectiles::OnDestroy(bool AbilityEnded)
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
        GetWorld()->GetTimerManager().ClearTimer(CompleteTimer);
    }
    Super::OnDestroy(AbilityEnded);
}