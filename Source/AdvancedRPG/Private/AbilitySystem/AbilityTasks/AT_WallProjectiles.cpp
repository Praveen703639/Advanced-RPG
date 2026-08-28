// pravin's  patashala all Rights Reserved 




#include "AbilitySystem/AbilityTasks/AT_WallProjectiles.h"
#include "Items/Projectiles/WarriorProjectileBase.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemComponent.h"

UAT_WallProjectiles::UAT_WallProjectiles()
    : NumProjectiles(8)
    , ProjectileSpacing(120.0f)
    , Speed(1600.0f)
    , SpawnDistance(0.0f)
    , ProjectileLifeSpan(3.0f)
    , ZOffset(0.0f)
{
}

UAT_WallProjectiles* UAT_WallProjectiles::WallProjectiles(
    UGameplayAbility* OwningAbility,
    TSubclassOf<AWarriorProjectileBase> InProjectileClass,
    FGameplayEffectSpecHandle InDamageEffectSpecHandle,
    UNiagaraSystem* InNiagaraEffect,
    FTransform InSpawnTransform,
    int32 InNumProjectiles,
    float InProjectileSpacing,
    float InSpeed,
    float InSpawnDistance,
    float InProjectileLifeSpan,
    float InZOffset)
{
    UAT_WallProjectiles* Task = NewAbilityTask<UAT_WallProjectiles>(OwningAbility);
    Task->ProjectileClass = InProjectileClass;
    Task->DamageEffectSpecHandle = InDamageEffectSpecHandle;
    Task->NiagaraEffect = InNiagaraEffect;
    Task->SpawnTransform = InSpawnTransform;
    Task->NumProjectiles = FMath::Clamp(InNumProjectiles, 1, 24);
    Task->ProjectileSpacing = InProjectileSpacing;
    Task->Speed = InSpeed;
    Task->SpawnDistance = InSpawnDistance;
    Task->ProjectileLifeSpan = InProjectileLifeSpan;
    Task->ZOffset = InZOffset;
    return Task;
}

void UAT_WallProjectiles::Activate()
{
    Super::Activate();
    SpawnWall();
}

void UAT_WallProjectiles::SpawnWall()
{
    AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
    if (!Avatar)
    {
        UE_LOG(LogTemp, Error, TEXT("WallProjectiles: Invalid Avatar!"));
        EndTask();
        return;
    }

    if (!ProjectileClass)
    {
        UE_LOG(LogTemp, Error, TEXT("WallProjectiles: ProjectileClass is NULL!"));
        EndTask();
        return;
    }

    FVector SpawnOrigin;
    FVector Forward;
    FVector Right;

    if (!SpawnTransform.Equals(FTransform::Identity))
    {
        SpawnOrigin = SpawnTransform.GetLocation();
        Forward = SpawnTransform.GetRotation().GetForwardVector();
        Right = SpawnTransform.GetRotation().GetRightVector();
    }
    else
    {
        SpawnOrigin = Avatar->GetActorLocation();
        Forward = Avatar->GetActorForwardVector();
        Right = Avatar->GetActorRightVector();
    }

    SpawnOrigin += (Forward * SpawnDistance) + FVector(0.0f, 0.0f, ZOffset);

    float TotalWidth = (NumProjectiles - 1) * ProjectileSpacing;
    FVector StartOffset = Right * (-TotalWidth * 0.5f);

    for (int32 i = 0; i < NumProjectiles; i++)
    {
        FVector SideOffset = Right * (i * ProjectileSpacing);
        FVector SpawnLoc = SpawnOrigin + SideOffset;
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
            SpawnedProjectiles.Add(Proj);
        }
    }

    GetWorld()->GetTimerManager().SetTimer(
        CompleteTimer,
        this,
        &UAT_WallProjectiles::CleanupAndEnd,
        ProjectileLifeSpan + 1.0f,
        false);
}

void UAT_WallProjectiles::SetupProjectile(AWarriorProjectileBase* Proj, const FVector& Direction)
{
    if (!Proj) return;

    FVector Velocity = Direction * Speed;

    Proj->SetProjectileVelocity(Velocity);
    Proj->SetProjectileSpeed(Speed, Speed * 1.2f);
    Proj->SetProjectileGravityScale(0.0f);

    Proj->SetDamageEffectSpecHandle(DamageEffectSpecHandle);

    if (NiagaraEffect)
    {
        Proj->SetNiagaraEffect(NiagaraEffect);
        Proj->ActivateNiagaraComponent(true);
    }

    Proj->SetActorRotation(Direction.Rotation());
    Proj->SetLifeSpan(ProjectileLifeSpan);
}

void UAT_WallProjectiles::CleanupAndEnd()
{
    GetWorld()->GetTimerManager().ClearTimer(CompleteTimer);

    for (AWarriorProjectileBase* Proj : SpawnedProjectiles)
    {
        if (IsValid(Proj))
        {
            Proj->Destroy();
        }
    }
    SpawnedProjectiles.Empty();

    OnWallComplete.Broadcast();
    EndTask();
}

void UAT_WallProjectiles::ExternalCancel()
{
    CleanupAndEnd();
    Super::ExternalCancel();
}

void UAT_WallProjectiles::OnDestroy(bool AbilityEnded)
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(CompleteTimer);
    }
    Super::OnDestroy(AbilityEnded);
}

