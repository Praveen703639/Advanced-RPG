#include "AbilitySystem/AbilityTasks/UAT_CircleRhythm.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"

UAT_CircleRhythm::UAT_CircleRhythm()
    : Duration(4.0f)
    , WolfOrbitRadius(300.0f)
    , TickInterval(1.0f)
    , BoxTraceExtent(75.0f)
    , NumWolves(3)  // ← DEFAULT TO 3 WOLVES
{
}

UAT_CircleRhythm* UAT_CircleRhythm::CircleRhythm(
    UGameplayAbility* OwningAbility,
    TSubclassOf<AActor> InWolfActorClass,
    int32 InNumWolves,
    float InDuration,
    float InWolfOrbitRadius,
    float InTickInterval,
    float InBoxTraceExtent)
{
    UAT_CircleRhythm* Task = NewAbilityTask<UAT_CircleRhythm>(OwningAbility);
    Task->WolfActorClass = InWolfActorClass;
    Task->NumWolves = InNumWolves;
    Task->Duration = InDuration;
    Task->WolfOrbitRadius = InWolfOrbitRadius;
    Task->TickInterval = InTickInterval;
    Task->BoxTraceExtent = InBoxTraceExtent;

    return Task;
}

void UAT_CircleRhythm::Activate()
{
    Super::Activate();
    StartWolfOrbit();
}

void UAT_CircleRhythm::StartWolfOrbit()
{
    if (!Ability || !Ability->GetCurrentActorInfo() || !Ability->GetCurrentActorInfo()->AvatarActor.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("CircleRhythm: Invalid Ability or Avatar!"));
        EndTask();
        return;
    }

    if (!WolfActorClass)
    {
        UE_LOG(LogTemp, Error, TEXT("CircleRhythm: WolfActorClass is NULL!"));
        EndTask();
        return;
    }

    if (NumWolves <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("CircleRhythm: NumWolves is %d, must be > 0!"), NumWolves);
        EndTask();
        return;
    }

    SpawnWolves();

    // Orbit timer (~60fps)
    GetWorld()->GetTimerManager().SetTimer(
        OrbitTimer,
        this,
        &UAT_CircleRhythm::UpdateWolfOrbits,
        0.016f,
        true);

    // Damage tick timer
    GetWorld()->GetTimerManager().SetTimer(
        DamageTickTimer,
        this,
        &UAT_CircleRhythm::TickDamage,
        TickInterval,
        true);

    // Duration timer
    GetWorld()->GetTimerManager().SetTimer(
        DurationTimer,
        this,
        &UAT_CircleRhythm::CleanupAndEnd,
        Duration,
        false);
}

void UAT_CircleRhythm::SpawnWolves()
{
    AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
    if (!Avatar)
    {
        UE_LOG(LogTemp, Error, TEXT("CircleRhythm: Avatar is NULL in SpawnWolves!"));
        return;
    }

    FVector AvatarLoc = Avatar->GetActorLocation();

    UE_LOG(LogTemp, Log, TEXT("CircleRhythm: Spawning %d wolves at radius %.0f"), NumWolves, WolfOrbitRadius);

    for (int32 i = 0; i < NumWolves; i++)
    {
        float BaseAngle = (360.0f / NumWolves) * i;
        WolfAngles.Add(BaseAngle);

        float Rad = FMath::DegreesToRadians(BaseAngle);
        FVector SpawnLoc = AvatarLoc + FVector(
            WolfOrbitRadius * FMath::Cos(Rad),
            WolfOrbitRadius * FMath::Sin(Rad),
            100.0f);  // ← Z offset so they don't spawn underground

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpawnParams.Owner = Avatar;

        AActor* Wolf = GetWorld()->SpawnActor<AActor>(WolfActorClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);

        if (Wolf)
        {
            FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(SpawnLoc, AvatarLoc);
            Wolf->SetActorRotation(LookAtRot);

            SpawnedWolves.Add(Wolf);
            UE_LOG(LogTemp, Log, TEXT("CircleRhythm: Spawned wolf %d at %s"), i, *SpawnLoc.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("CircleRhythm: FAILED to spawn wolf %d!"), i);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("CircleRhythm: Total spawned wolves: %d"), SpawnedWolves.Num());
}

void UAT_CircleRhythm::UpdateWolfOrbits()
{
    AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
    if (!Avatar)
    {
        return;
    }

    FVector Center = Avatar->GetActorLocation();
    float DeltaTime = GetWorld()->GetDeltaSeconds();
    float OrbitSpeed = 90.0f;

    for (int32 i = 0; i < SpawnedWolves.Num(); i++)
    {
        AActor* Wolf = SpawnedWolves[i];
        if (!IsValid(Wolf))
        {
            continue;
        }

        WolfAngles[i] += OrbitSpeed * DeltaTime;
        if (WolfAngles[i] >= 360.0f)
        {
            WolfAngles[i] -= 360.0f;
        }

        float Rad = FMath::DegreesToRadians(WolfAngles[i]);
        FVector NewLoc = Center + FVector(
            WolfOrbitRadius * FMath::Cos(Rad),
            WolfOrbitRadius * FMath::Sin(Rad),
            100.0f);  // ← Keep same Z offset

        Wolf->SetActorLocation(NewLoc);

        float TangentRad = Rad + PI / 2.0f;
        FRotator TangentRot = FRotator(0.0f, FMath::RadiansToDegrees(TangentRad), 0.0f);
        Wolf->SetActorRotation(TangentRot);
    }
}

void UAT_CircleRhythm::TickDamage()
{
    for (AActor* Wolf : SpawnedWolves)
    {
        if (IsValid(Wolf))
        {
            PerformBoxTraceForWolf(Wolf);
        }
    }
}

void UAT_CircleRhythm::PerformBoxTraceForWolf(AActor* Wolf)
{
    if (!Wolf || !Ability)
    {
        return;
    }

    AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
    if (!Avatar)
    {
        return;
    }

    FVector TraceCenter = Wolf->GetActorLocation();

    TArray<FHitResult> HitResults;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Avatar);
    ActorsToIgnore.Append(SpawnedWolves);

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    UKismetSystemLibrary::BoxTraceMultiForObjects(
        GetWorld(),
        TraceCenter,
        TraceCenter,
        FVector(BoxTraceExtent, BoxTraceExtent, BoxTraceExtent / 2.0f),
        Wolf->GetActorRotation(),
        ObjectTypes,
        false,
        ActorsToIgnore,
        EDrawDebugTrace::None,
        HitResults,
        true);

    if (HitResults.Num() == 0)
    {
        return;
    }

    for (const FHitResult& Hit : HitResults)
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor || HitActor == Avatar)
        {
            continue;
        }

        OnWolfHitEnemy.Broadcast(HitActor);
        UE_LOG(LogTemp, Log, TEXT("CircleRhythm: Wolf hit enemy %s"), *HitActor->GetName());
    }
}

void UAT_CircleRhythm::CleanupAndEnd()
{
    GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

    for (AActor* Wolf : SpawnedWolves)
    {
        if (IsValid(Wolf))
        {
            Wolf->Destroy();
        }
    }
    SpawnedWolves.Empty();
    WolfAngles.Empty();

    OnCircleRhythmComplete.Broadcast();
    EndTask();
}

void UAT_CircleRhythm::ExternalCancel()
{
    CleanupAndEnd();
    Super::ExternalCancel();
}

void UAT_CircleRhythm::OnDestroy(bool AbilityEnded)
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
    }

    Super::OnDestroy(AbilityEnded);
}