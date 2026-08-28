// pravin's patashala all Rights Reserved

#include "AbilitySystem/AbilityTasks/AbilityTask_TransformCharacter.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraActor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"
#include "Characters/WarriorBowCharacter.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Items/Weapons/WarriorWeaponBase.h"
#include "Components/Combat/BowCombatComponent.h"

// ========================== CREATE TASK ==========================
UAbilityTask_TransformCharacter* UAbilityTask_TransformCharacter::TransformCharacter(
    UGameplayAbility* OwningAbility,
    ETransformType TransformType,
    TSoftClassPtr<AWarriorHeroCharacter> HeroClass,
    TSoftClassPtr<AWarriorBowCharacter> BowClass,
    UAnimMontage* SourceOutMontage,
    UAnimMontage* TargetInMontage,
    UParticleSystem* DissolveEffect,
    UParticleSystem* FormEffect,
    float TransformDuration,
    bool bUseCinematicCamera,
    TSubclassOf<ACameraActor> CinematicCameraClass)
{
    UAbilityTask_TransformCharacter* Node = NewAbilityTask<UAbilityTask_TransformCharacter>(OwningAbility);

    if (Node)
    {
        Node->CachedTransformType = TransformType;
        Node->CachedHeroClass = HeroClass;
        Node->CachedBowClass = BowClass;
        Node->CachedSourceOutMontage = SourceOutMontage;
        Node->CachedTargetInMontage = TargetInMontage;
        Node->CachedDissolveEffect = DissolveEffect;
        Node->CachedFormEffect = FormEffect;
        Node->CachedTransformDuration = FMath::Max(TransformDuration, 1.0f);
        Node->bCachedUseCinematicCamera = bUseCinematicCamera;
        Node->CachedCinematicCameraClass = CinematicCameraClass;
    }

    return Node;
}

// ========================== SET WEAPON TO TRANSFER ==========================
void UAbilityTask_TransformCharacter::SetWeaponToTransfer(AWarriorWeaponBase* Weapon, FGameplayTag WeaponTag)
{
    WeaponToTransfer = Weapon;
    TransferWeaponTag = WeaponTag;
}

// ========================== SET CAMERA SHAKE PARAMS ==========================
void UAbilityTask_TransformCharacter::SetCameraShakeParams(float Intensity, float Duration, float BlendIn, float BlendOut)
{
    // Reserved for future implementation
}

// ========================== ACTIVATE ==========================
void UAbilityTask_TransformCharacter::Activate()
{
    // Reset all state flags to handle task reuse or reactivation
    bIsTransforming = false;
    bTransformFailed = false;
    bHasSpawnedNewCharacter = false;
    bIsRestoringControl = false;

    if (!AbilitySystemComponent.IsValid())
    {
        OnTransformFailed.Broadcast();
        EndTask();
        return;
    }

    SourceCharacter = AbilitySystemComponent->GetAvatarActor();
    if (!SourceCharacter)
    {
        OnTransformFailed.Broadcast();
        EndTask();
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        OnTransformFailed.Broadcast();
        EndTask();
        return;
    }

    OwningController = UGameplayStatics::GetPlayerController(World, 0);

    CachedTransformLocation = SourceCharacter->GetActorLocation();
    CachedTransformRotation = SourceCharacter->GetActorRotation();
    CachedSpawnTransform = SourceCharacter->GetActorTransform();

    // Load required class synchronously with validation
    if (CachedTransformType == ETransformType::HeroToBow)
    {
        CachedBowClass.LoadSynchronous();
        if (!CachedBowClass.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("AbilityTask_TransformCharacter: Failed to load BowClass!"));
            OnTransformFailed.Broadcast();
            EndTask();
            return;
        }
    }
    else
    {
        CachedHeroClass.LoadSynchronous();
        if (!CachedHeroClass.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("AbilityTask_TransformCharacter: Failed to load HeroClass!"));
            OnTransformFailed.Broadcast();
            EndTask();
            return;
        }
    }

    StartTransformSequence();
}

// ========================== START SEQUENCE ==========================
void UAbilityTask_TransformCharacter::StartTransformSequence()
{
    if (bIsTransforming)
    {
        return;
    }
    bIsTransforming = true;

    UWorld* World = GetWorld();
    if (!World)
    {
        OnTransformFailed.Broadcast();
        EndTask();
        return;
    }

    // Handle Cinematic Camera setup
    if (bCachedUseCinematicCamera && OwningController && CachedCinematicCameraClass)
    {
        FVector CamLoc = CachedTransformLocation + CachedCameraOffset;
        FRotator CamRot = (CachedTransformLocation - CamLoc).Rotation();

        FActorSpawnParameters CamParams;
        CamParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        CinematicCamera = World->SpawnActor<ACameraActor>(CachedCinematicCameraClass, CamLoc, CamRot, CamParams);

        if (CinematicCamera)
        {
            OwningController->SetViewTargetWithBlend(CinematicCamera.Get(), CameraBlendInTime);
        }
    }

    // Disable character movement and collision
    if (ACharacter* Char = Cast<ACharacter>(SourceCharacter))
    {
        if (UCharacterMovementComponent* Move = Char->GetCharacterMovement())
        {
            Move->DisableMovement();
            Move->StopMovementImmediately();
        }
        Char->SetActorEnableCollision(false);
    }

    PlayOutMontageAndDissolve();
}

// ========================== OUT ANIMATION ==========================
void UAbilityTask_TransformCharacter::PlayOutMontageAndDissolve()
{
    ACharacter* Char = Cast<ACharacter>(SourceCharacter);
    if (!Char)
    {
        OnTransformFailed.Broadcast();
        EndTask();
        return;
    }

    UAnimInstance* Anim = Char->GetMesh()->GetAnimInstance();
    if (!Anim)
    {
        OnTransformFailed.Broadcast();
        EndTask();
        return;
    }

    float MontageLength = 0.0f;

    if (CachedSourceOutMontage)
    {
        MontageLength = Anim->Montage_Play(CachedSourceOutMontage);

        FOnMontageEnded EndDelegate;
        EndDelegate.BindUObject(this, &UAbilityTask_TransformCharacter::OnOutMontageEnded);
        Anim->Montage_SetEndDelegate(EndDelegate, CachedSourceOutMontage);
    }

    if (CachedDissolveEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CachedDissolveEffect, CachedTransformLocation);
    }

    // Calculate spawn time: 80% through montage, or minimum fallback
    float SpawnTime = FMath::Max(MontageLength * MontageToSpawnRatio, MinSpawnDelay);

    GetWorld()->GetTimerManager().SetTimer(
        SpawnTimer,
        this,
        &UAbilityTask_TransformCharacter::SpawnNewCharacter,
        SpawnTime,
        false);
}

// ========================== OUT END ==========================
void UAbilityTask_TransformCharacter::OnOutMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    // Always clear the timer to prevent double-spawn
    World->GetTimerManager().ClearTimer(SpawnTimer);

    if (bInterrupted)
    {
        bTransformFailed = true;
        OnTransformFailed.Broadcast();
        EndTask();
        return;
    }

    // Guard: only spawn if timer hasn't already fired
    if (!bHasSpawnedNewCharacter)
    {
        SpawnNewCharacter();
    }
}

// ========================== SPAWN NEW CHARACTER ==========================
// ========================== SPAWN NEW CHARACTER ==========================
// ========================== SPAWN NEW CHARACTER ==========================
// ========================== SPAWN NEW CHARACTER ==========================
void UAbilityTask_TransformCharacter::SpawnNewCharacter()
{
    if (bHasSpawnedNewCharacter)
    {
        return;
    }
    bHasSpawnedNewCharacter = true;

    if (bTransformFailed || !SourceCharacter)
    {
        EndTask();
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        OnTransformFailed.Broadcast();
        EndTask();
        return;
    }

    ACharacter* OldChar = Cast<ACharacter>(SourceCharacter);
    AController* Controller = OldChar ? OldChar->GetController() : nullptr;

    // DO NOT touch WeaponToTransfer here. GA already destroyed hero weapons.

    SourceCharacter->Destroy();
    SourceCharacter = nullptr;

    FVector Loc = CachedTransformLocation;
    FRotator Rot = CachedTransformRotation;

    UClass* SpawnClass = (CachedTransformType == ETransformType::HeroToBow)
        ? CachedBowClass.Get()
        : CachedHeroClass.Get();

    if (!SpawnClass)
    {
        OnTransformFailed.Broadcast();
        EndTask();
        return;
    }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ACharacter* NewChar = World->SpawnActor<ACharacter>(SpawnClass, Loc, Rot, Params);
    if (!NewChar)
    {
        OnTransformFailed.Broadcast();
        EndTask();
        return;
    }

    SpawnedCharacter = NewChar;
    NewChar->SetActorEnableCollision(true);

    if (Controller)
    {
        Controller->UnPossess();
        if (Controller)
        {
            Controller->Possess(NewChar);
        }
    }

    World->GetTimerManager().SetTimer(
        FormTimer,
        this,
        &UAbilityTask_TransformCharacter::PlayInMontageAndForm,
        PostPossessDelay,
        false);
}

// ========================== IN ANIMATION ==========================
void UAbilityTask_TransformCharacter::PlayInMontageAndForm()
{
    UE_LOG(LogTemp, Warning, TEXT("=== PLAY IN MONTAGE ==="));
    UE_LOG(LogTemp, Warning, TEXT("SpawnedCharacter: %s"), SpawnedCharacter ? *SpawnedCharacter->GetName() : TEXT("NULL"));

    if (!SpawnedCharacter)
    {
        OnTransformFailed.Broadcast();
        EndTask();
        return;
    }

    ACharacter* Char = Cast<ACharacter>(SpawnedCharacter);
    if (!Char)
    {
        RestorePlayerControl();
        return;
    }

    // ============================================
    // DO NOT transfer hero weapon to bow here
    // Bow spawns its own arrow via SpawnDefaultBowArrow() in HandleTransformComplete
    // Hero weapon is NOT an arrow — different class entirely
    // ============================================

    UAnimInstance* Anim = Char->GetMesh()->GetAnimInstance();
    if (!Anim)
    {
        RestorePlayerControl();
        return;
    }

    float Length = 0.0f;

    if (CachedTargetInMontage)
    {
        Length = Anim->Montage_Play(CachedTargetInMontage);

        FOnMontageEnded EndDelegate;
        EndDelegate.BindUObject(this, &UAbilityTask_TransformCharacter::OnInMontageEnded);
        Anim->Montage_SetEndDelegate(EndDelegate, CachedTargetInMontage);
    }

    // Spawn form effect attached to new character mesh
    if (CachedFormEffect)
    {
        SpawnedFormEffectComponent = UGameplayStatics::SpawnEmitterAttached(
            CachedFormEffect,
            Char->GetMesh(),
            NAME_None,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::SnapToTarget,
            true
        );
    }

    float RestoreTime = FMath::Max(Length, MinRestoreControlDelay);

    GetWorld()->GetTimerManager().SetTimer(
        FormTimer,
        this,
        &UAbilityTask_TransformCharacter::RestorePlayerControl,
        RestoreTime,
        false);
}

// ========================== IN MONTAGE ENDED ==========================
void UAbilityTask_TransformCharacter::OnInMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    UWorld* World = GetWorld();
    if (World)
    {
        World->GetTimerManager().ClearTimer(FormTimer);
    }

    RestorePlayerControl();
}

// ========================== RESTORE CONTROL ==========================
void UAbilityTask_TransformCharacter::RestorePlayerControl()
{
    if (bIsRestoringControl)
    {
        return;
    }
    bIsRestoringControl = true;

    UWorld* World = GetWorld();

    // Clear any pending timers
    if (World)
    {
        World->GetTimerManager().ClearTimer(SpawnTimer);
        World->GetTimerManager().ClearTimer(FormTimer);
    }

    // Kill and destroy the form-effect component immediately
    if (IsValid(SpawnedFormEffectComponent))
    {
        SpawnedFormEffectComponent->KillParticlesForced();
        SpawnedFormEffectComponent->DestroyComponent();
        SpawnedFormEffectComponent = nullptr;
    }

    // Restore player camera
    if (OwningController && CinematicCamera)
    {
        OwningController->SetViewTargetWithBlend(OwningController->GetPawn(), CameraBlendOutTime);
    }

    CleanupCinematicCamera();

    // Ensure spawned character has proper movement restored
    if (ACharacter* SpawnedChar = Cast<ACharacter>(SpawnedCharacter))
    {
        if (UCharacterMovementComponent* Move = SpawnedChar->GetCharacterMovement())
        {
            Move->SetMovementMode(MOVE_Walking);
        }
        SpawnedChar->SetActorEnableCollision(true);
    }

    // Broadcast success with the new character before ending task
    OnTransformSucceeded.Broadcast(SpawnedCharacter);

    EndTask();
}

// ========================== CLEANUP CINEMATIC CAMERA ==========================
void UAbilityTask_TransformCharacter::CleanupCinematicCamera()
{
    if (CinematicCamera)
    {
        CinematicCamera->Destroy();
        CinematicCamera = nullptr;
    }
}

// ========================== DESTROY ==========================
void UAbilityTask_TransformCharacter::OnDestroy(bool bInOwnerFinished)
{
    UWorld* World = GetWorld();
    if (World)
    {
        World->GetTimerManager().ClearTimer(SpawnTimer);
        World->GetTimerManager().ClearTimer(FormTimer);
    }

    CleanupCinematicCamera();

    // If task is destroyed mid-transformation, try to restore movement on source
    if (ACharacter* Char = Cast<ACharacter>(SourceCharacter))
    {
        if (UCharacterMovementComponent* Move = Char->GetCharacterMovement())
        {
            Move->SetMovementMode(MOVE_Walking);
        }
        Char->SetActorEnableCollision(true);
    }

    // Ensure spawned character has movement restored if control wasn't properly handed back
    if (ACharacter* SpawnedChar = Cast<ACharacter>(SpawnedCharacter))
    {
        if (UCharacterMovementComponent* Move = SpawnedChar->GetCharacterMovement())
        {
            Move->SetMovementMode(MOVE_Walking);
        }
        SpawnedChar->SetActorEnableCollision(true);
    }

    bIsTransforming = false;
    SourceCharacter = nullptr;

    Super::OnDestroy(bInOwnerFinished);
}