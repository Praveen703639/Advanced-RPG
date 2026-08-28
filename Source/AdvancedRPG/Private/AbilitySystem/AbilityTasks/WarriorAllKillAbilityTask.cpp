// WarriorAllKillAbilityTask.cpp
#include "AbilitySystem/AbilityTasks/WarriorAllKillAbilityTask.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "AbilitySystemComponent.h"

UAT_AllKill* UAT_AllKill::AllKill(
    UGameplayAbility* OwningAbility,
    UParticleSystem* InWolfEyeParticle,
    UParticleSystem* InPortalParticle,
    TSubclassOf<AActor> InWolfMesh,
    UAnimMontage* InWolfHopMontage)
{
    UAT_AllKill* Task = NewAbilityTask<UAT_AllKill>(OwningAbility);
    Task->WolfEyeParticle = InWolfEyeParticle;
    Task->PortalParticle = InPortalParticle;
    Task->WolfSkeletalMeshClass = InWolfMesh;
    Task->WolfHopMontage = InWolfHopMontage;

    Task->TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    return Task;
}

void UAT_AllKill::Activate()
{
    if (!Ability || !Ability->GetCurrentActorInfo()->AvatarActor.IsValid())
    {
        EndTask();
        return;
    }

    if (!FindTargetViaBoxTrace())
    {
        UE_LOG(LogTemp, Warning, TEXT("AllKill: No target found via box trace!"));
        EndTask();
        return;
    }

    TargetLocation = TargetActor->GetActorLocation();

    // Step 1: Spawn eyes at random locations
    SpawnWolfEyes();

    // Step 2: After eyes glow, start spawning wolves from each eye location
    // Each spawn: portal opens at eye location → wolf spawns from portal
    CurrentWolfIndex = 0;
    GetWorld()->GetTimerManager().SetTimer(
        WolfSpawnTimer,
        this,
        &UAT_AllKill::SpawnNextWolf,
        WolfSpawnInterval,
        true
    );
}

bool UAT_AllKill::FindTargetViaBoxTrace()
{
    AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
    if (!Avatar)
    {
        return false;
    }

    FVector TraceStart = Avatar->GetActorLocation();
    FVector TraceEnd = TraceStart + (Avatar->GetActorForwardVector() * TraceRange);

    TArray<FHitResult> HitResults;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Avatar);

    UKismetSystemLibrary::BoxTraceMultiForObjects(
        GetWorld(),
        TraceStart,
        TraceEnd,
        TraceBoxExtent,
        Avatar->GetActorRotation(),
        TraceObjectTypes,
        false,
        ActorsToIgnore,
        EDrawDebugTrace::None,
        HitResults,
        true
    );

    if (HitResults.Num() == 0)
    {
        return false;
    }

    // === CHANGED: Use AActor* so non-WarriorEnemyCharacter Bosses (e.g. air enemies) are valid ===
    TArray<AActor*> Bosses;
    TArray<AActor*> RegularEnemies;

    for (const FHitResult& Hit : HitResults)
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor || HitActor == Avatar)
        {
            continue;
        }

        // === CHANGED: Check IsBoss() OR Actor Tag "Boss" ===
        if (IsBossCharacter(HitActor))
        {
            Bosses.Add(HitActor);
        }
        else
        {
            // Only WarriorEnemyCharacters count as regular enemies
            AWarriorEnemyCharacter* EnemyChar = Cast<AWarriorEnemyCharacter>(HitActor);
            if (EnemyChar)
            {
                RegularEnemies.Add(HitActor);
            }
        }
    }

    // ===========================
    // Bosses have highest priority
    // ===========================
    if (Bosses.Num() > 0)
    {
        float ClosestDist = MAX_flt;
        AActor* ClosestBoss = nullptr;

        for (AActor* Boss : Bosses)
        {
            if (!Boss)
            {
                continue;
            }

            float Dist = FVector::Dist(
                Avatar->GetActorLocation(),
                Boss->GetActorLocation());

            if (Dist < ClosestDist)
            {
                ClosestDist = Dist;
                ClosestBoss = Boss;
            }
        }

        if (ClosestBoss)
        {
            TargetActor = ClosestBoss;
            TargetLocation = ClosestBoss->GetActorLocation();

            UE_LOG(LogTemp, Log,
                TEXT("AllKill: Targeting BOSS - %s"),
                *ClosestBoss->GetName());

            return true;
        }
    }

    // ===========================
    // Otherwise target closest enemy
    // ===========================
    if (RegularEnemies.Num() > 0)
    {
        float ClosestDist = MAX_flt;
        AActor* ClosestEnemy = nullptr;

        for (AActor* Enemy : RegularEnemies)
        {
            if (!Enemy)
            {
                continue;
            }

            float Dist = FVector::Dist(
                Avatar->GetActorLocation(),
                Enemy->GetActorLocation());

            if (Dist < ClosestDist)
            {
                ClosestDist = Dist;
                ClosestEnemy = Enemy;
            }
        }

        if (ClosestEnemy)
        {
            TargetActor = ClosestEnemy;
            TargetLocation = ClosestEnemy->GetActorLocation();

            UE_LOG(LogTemp, Log,
                TEXT("AllKill: Targeting ENEMY - %s"),
                *ClosestEnemy->GetName());

            return true;
        }
    }

    return false;
}

// === CHANGED: Now respects Actor Tag "Boss" for any AActor ===
bool UAT_AllKill::IsBossCharacter(AActor* Actor)
{
    if (!Actor) return false;

    AWarriorEnemyCharacter* EnemyChar = Cast<AWarriorEnemyCharacter>(Actor);
    if (EnemyChar && EnemyChar->IsBoss()) return true;

    return Actor->ActorHasTag(FName("Boss"));
}

void UAT_AllKill::SpawnWolfEyes()
{
    AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();

    for (int32 i = 0; i < NumWolfEyes; i++)
    {
        // Get random location for this eye
        FVector SpawnLoc = GetRandomEyeLocation();

        // Store location for portal + wolf spawning
        WolfEyeLocations.Add(SpawnLoc);

        // Spawn eye particle at this location
        UParticleSystemComponent* EyeParticle = UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            WolfEyeParticle,
            SpawnLoc,
            FRotator::ZeroRotator,
            true
        );

        if (EyeParticle)
        {
            // Make eyes face the target
            FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(SpawnLoc, TargetLocation);
            EyeParticle->SetWorldRotation(LookAtRot);

            SpawnedWolfEyes.Add(EyeParticle);
        }
    }
}

FVector UAT_AllKill::GetRandomEyeLocation()
{
    AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
    FVector Center = Avatar->GetActorLocation();

    // Eyes in a circle around Shinbi
    float Angle = FMath::RandRange(0.0f, 360.0f);
    float Radius = FMath::RandRange(WolfEyeSpawnRadius * 0.5f, WolfEyeSpawnRadius);

    float X = Center.X + Radius * FMath::Cos(FMath::DegreesToRadians(Angle));
    float Y = Center.Y + Radius * FMath::Sin(FMath::DegreesToRadians(Angle));

    // Height variation for scattered look
    float Z = Center.Z + FMath::RandRange(WolfEyeSpawnHeightMin, WolfEyeSpawnHeightMax);

    return FVector(X, Y, Z);
}

void UAT_AllKill::SpawnNextWolf()
{
    if (CurrentWolfIndex >= NumWolfEyes || CurrentWolfIndex >= WolfEyeLocations.Num())
    {
        GetWorld()->GetTimerManager().ClearTimer(WolfSpawnTimer);

        FTimerHandle CleanupTimer;
        GetWorld()->GetTimerManager().SetTimer(
            CleanupTimer,
            this,
            &UAT_AllKill::CleanupAndEnd,
            2.0f,
            false
        );
        return;
    }

    AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();

    // Get the eye location for this wolf
    FVector EyeLoc = WolfEyeLocations[CurrentWolfIndex];

    // === STEP 1: Spawn portal at eye location ===
    UParticleSystemComponent* Portal = UGameplayStatics::SpawnEmitterAtLocation(
        GetWorld(),
        PortalParticle,
        EyeLoc,
        FRotator::ZeroRotator,
        true
    );

    if (Portal)
    {
        SpawnedPortals.Add(Portal);
    }

    // === STEP 2: Spawn wolf from portal (eye location) ===
    FVector RandomOffset = FVector(
        FMath::RandRange(-30.0f, 30.0f),
        FMath::RandRange(-30.0f, 30.0f),
        0.0f
    );

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.Owner = Avatar;

    AActor* Wolf = GetWorld()->SpawnActor<AActor>(
        WolfSkeletalMeshClass,
        EyeLoc + RandomOffset,
        FRotator::ZeroRotator,
        SpawnParams
    );

    if (Wolf)
    {
        // Face the target
        FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
            Wolf->GetActorLocation(),
            TargetLocation
        );
        Wolf->SetActorRotation(LookAtRot);

        SpawnedWolves.Add(Wolf);

        // Play hop montage
        ACharacter* WolfChar = Cast<ACharacter>(Wolf);
        if (WolfChar && WolfHopMontage)
        {
            UAnimInstance* AnimInst = WolfChar->GetMesh()->GetAnimInstance();
            if (AnimInst)
            {
                AnimInst->Montage_Play(WolfHopMontage);
            }
        }

        // Launch wolf from eye location toward target
        LaunchWolfToTarget(Wolf, EyeLoc);
    }

    CurrentWolfIndex++;
}

void UAT_AllKill::LaunchWolfToTarget(AActor* Wolf, FVector StartLocation)
{
    if (!Wolf || !TargetActor.IsValid()) return;

    USkeletalMeshComponent* WolfMesh = Wolf->FindComponentByClass<USkeletalMeshComponent>();
    if (!WolfMesh) return;

    FVector EndLoc = TargetLocation;

    EndLoc += FVector(
        FMath::RandRange(-75.0f, 75.0f),
        FMath::RandRange(-75.0f, 75.0f),
        0.0f
    );

    float Distance = FVector::Dist2D(StartLocation, EndLoc);
    float TimeToTarget = Distance / WolfHopSpeed;

    FVector Velocity;
    Velocity.X = (EndLoc.X - StartLocation.X) / TimeToTarget;
    Velocity.Y = (EndLoc.Y - StartLocation.Y) / TimeToTarget;

    float Gravity = GetWorld()->GetGravityZ() * -1;
    Velocity.Z = (EndLoc.Z - StartLocation.Z + (0.5f * Gravity * TimeToTarget * TimeToTarget)) / TimeToTarget;
    Velocity.Z += WolfHopArcHeight;

    WolfMesh->SetSimulatePhysics(true);
    WolfMesh->SetAllPhysicsLinearVelocity(Velocity);

    // Safe damage timer
    TWeakObjectPtr<AActor> WeakWolf = Wolf;
    TWeakObjectPtr<AActor> WeakTarget = TargetActor.Get();

    FTimerHandle DamageTimer;
    FTimerDelegate DamageDelegate;
    DamageDelegate.BindLambda([this, WeakWolf, WeakTarget, TimeToTarget]()
        {
            if (!IsValid(this)) return;
            if (!WeakWolf.IsValid()) return;

            if (!WeakTarget.IsValid())
            {
                if (WeakWolf.IsValid()) WeakWolf->Destroy();
                return;
            }

            AActor* WolfActor = WeakWolf.Get();
            AActor* TargetActorPtr = WeakTarget.Get();

            // === BROADCAST WOLF HIT ===
            OnWolfHit.Broadcast(TargetActorPtr);

            if (Ability && Ability->GetCurrentActorInfo()->PlayerController.IsValid())
            {
                UGameplayStatics::ApplyDamage(
                    TargetActorPtr,
                    50.0f,
                    Ability->GetCurrentActorInfo()->PlayerController.Get(),
                    WolfActor,
                    nullptr
                );
            }

            if (WolfActor)
            {
                WolfActor->Destroy();
            }
        });

    GetWorld()->GetTimerManager().SetTimer(
        DamageTimer,
        DamageDelegate,
        TimeToTarget,
        false
    );
}

void UAT_AllKill::CleanupAndEnd()
{
    GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

    // Destroy eyes
    for (UParticleSystemComponent* Eye : SpawnedWolfEyes)
    {
        if (IsValid(Eye))
        {
            Eye->DeactivateSystem();
            Eye->DestroyComponent();
        }
    }
    SpawnedWolfEyes.Empty();

    // Destroy portals
    for (UParticleSystemComponent* Portal : SpawnedPortals)
    {
        if (IsValid(Portal))
        {
            Portal->DeactivateSystem();
            Portal->DestroyComponent();
        }
    }
    SpawnedPortals.Empty();

    // Destroy wolves
    for (AActor* Wolf : SpawnedWolves)
    {
        if (IsValid(Wolf))
        {
            Wolf->Destroy();
        }
    }
    SpawnedWolves.Empty();

    WolfEyeLocations.Empty();

    // === FIX: Broadcast with target actor ===
    AActor* HitActor = TargetActor.IsValid() ? TargetActor.Get() : nullptr;
    OnAllKillComplete.Broadcast(HitActor);

    EndTask();
}

void UAT_AllKill::ExternalCancel()
{
    CleanupAndEnd();
    Super::ExternalCancel();
}

void UAT_AllKill::OnDestroy(bool AbilityEnded)
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
    }

    Super::OnDestroy(AbilityEnded);
}