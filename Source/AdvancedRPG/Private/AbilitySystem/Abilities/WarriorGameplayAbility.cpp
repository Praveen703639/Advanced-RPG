// pravin's patashala all Rights Reserved

#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_TransformCharacter.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Characters/WarriorBowCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "WarriorFunctionLibrary.h"
#include "warriorGameplayTags.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Controllers/WarriorAIController.h"
#include "Animation/AnimInstance.h"

// 🔥 ADDED: Full include needed for DetachFromActor, SetActorHiddenInGame, etc.
#include "Items/Weapons/WarriorWeaponBase.h"

UWarriorGameplayAbility::UWarriorGameplayAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UWarriorGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);

    if (ActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven && !Spec.IsActive())
    {
        ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
    }
}

void UWarriorGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    // ✅ ADD THIS LINE at the top
    UnfreezeAllTrackedEnemies();

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

    if (ActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven)
    {
        ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
    }
}

EWarriorTransformType UWarriorGameplayAbility::GetAutoTransformType() const
{
    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar)
    {
        return EWarriorTransformType::None;
    }

    if (Avatar->IsA<AWarriorHeroCharacter>())
    {
        return EWarriorTransformType::HeroToBow;
    }
    else if (Avatar->IsA<AWarriorBowCharacter>())
    {
        return EWarriorTransformType::BowToHero;
    }

    return EWarriorTransformType::None;
}

void UWarriorGameplayAbility::ExecuteTransform(EWarriorTransformType TransformType)
{
    if (TransformType == EWarriorTransformType::None)
    {
        TransformType = GetAutoTransformType();
    }

    if (TransformType == EWarriorTransformType::None)
    {
        OnTransformFailed();
        return;
    }

    // Capture equipped weapon BEFORE creating task
    AWarriorWeaponBase* WeaponToTransfer = nullptr;
    FGameplayTag WeaponTag;
    AWarriorWeaponBase* EquippedWeapon = nullptr; // Declared outside the if block

    if (AActor* SourceActor = GetAvatarActorFromActorInfo())
    {
        if (UPawnCombatComponent* CombatComp = SourceActor->FindComponentByClass<UPawnCombatComponent>())
        {
            EquippedWeapon = CombatComp->GetCharacterCurrentEquippedWeapon();

            if (EquippedWeapon)
            {
                WeaponToTransfer = EquippedWeapon;
                WeaponTag = CombatComp->CurrentEquippedWeaponTag;

                // Detach and hide before transformation
                EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
                EquippedWeapon->SetActorHiddenInGame(true);
                EquippedWeapon->SetActorEnableCollision(false);

                UE_LOG(LogTemp, Warning, TEXT("ExecuteTransform: Captured weapon '%s'"), *EquippedWeapon->GetName());
            }

            // Hide all other weapons
            for (auto& WeaponPair : CombatComp->GetCharacterCarriedWeaponMap())
            {
                if (AWarriorWeaponBase* Weapon = WeaponPair.Value)
                {
                    if (Weapon != EquippedWeapon)
                    {
                        Weapon->SetActorHiddenInGame(true);
                        Weapon->SetActorEnableCollision(false);
                    }
                }
            }
        }
    }

    // Pick montages based on direction
    UAnimMontage* SourceOutMontage = nullptr;
    UAnimMontage* TargetInMontage = nullptr;

    switch (TransformType)
    {
    case EWarriorTransformType::HeroToBow:
        SourceOutMontage = HeroToBow_OutMontage;
        TargetInMontage = HeroToBow_InMontage;
        break;
    case EWarriorTransformType::BowToHero:
        SourceOutMontage = BowToHero_OutMontage;
        TargetInMontage = BowToHero_InMontage;
        break;
    default:
        break;
    }

    // Map to AbilityTask enum
    ETransformType TaskTransformType = (TransformType == EWarriorTransformType::HeroToBow)
        ? ETransformType::HeroToBow
        : ETransformType::BowToHero;

    // Spawn the task
    UAbilityTask_TransformCharacter* TransformTask =
        UAbilityTask_TransformCharacter::TransformCharacter(
            this,
            TaskTransformType,
            HeroClass,
            BowClass,
            SourceOutMontage,
            TargetInMontage,
            DissolveEffect,
            FormEffect,
            TransformDuration,
            true,
            CinematicCameraClass
        );

    if (!TransformTask)
    {
        // Restore weapon on failure
        if (WeaponToTransfer)
        {
            WeaponToTransfer->SetActorHiddenInGame(false);
            WeaponToTransfer->SetActorEnableCollision(true);
        }
        OnTransformFailed();
        return;
    }

    // Pass captured weapon to task
    if (WeaponToTransfer && WeaponTag.IsValid())
    {
        TransformTask->SetWeaponToTransfer(WeaponToTransfer, WeaponTag);
    }

    // Optional camera juice
    TransformTask->SetCameraShakeParams(2.0f, 1.5f, 110.f, 65.f);

    // Bind delegates
    TransformTask->OnTransformSucceeded.AddDynamic(this, &UWarriorGameplayAbility::OnTransformComplete);
    TransformTask->OnTransformFailed.AddDynamic(this, &UWarriorGameplayAbility::OnTransformFailed);
    TransformTask->ReadyForActivation();
}

UPawnCombatComponent* UWarriorGameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
    return GetAvatarActorFromActorInfo()->FindComponentByClass<UPawnCombatComponent>();
}

UWarriorAbilitySystemComponent* UWarriorGameplayAbility::GetWarriorAbilitySystemComponentFromActorInfo() const
{
    return Cast<UWarriorAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}

FActiveGameplayEffectHandle UWarriorGameplayAbility::NativeApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle)
{
    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

    if (TargetASC && InSpecHandle.IsValid())
    {
        FActiveGameplayEffectHandle ActiveHandle = GetWarriorAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);

        FGameplayEventData EventData;
        EventData.Instigator = GetAvatarActorFromActorInfo();
        EventData.Target = TargetActor;

        return ActiveHandle;
    }

    return FActiveGameplayEffectHandle();
}

FActiveGameplayEffectHandle UWarriorGameplayAbility::BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle, EWarriorSuccessType& OutSuccessType)
{
    FActiveGameplayEffectHandle ActiveGameplayEffect = NativeApplyEffectSpecHandleToTarget(TargetActor, InSpecHandle);
    OutSuccessType = ActiveGameplayEffect.WasSuccessfullyApplied() ? EWarriorSuccessType::Successful : EWarriorSuccessType::Failed;
    return ActiveGameplayEffect;
}

void UWarriorGameplayAbility::ApplyGameplayEfffectSpecHandleToHitResult(const FGameplayEffectSpecHandle& InSpecHandle, const TArray<FHitResult>& InHitResults)
{
    if (!InSpecHandle.IsValid())
    {
        return;
    }
    if (InHitResults.IsEmpty())
    {
        return;
    }

    APawn* OwningPawn = CastChecked<APawn>(GetAvatarActorFromActorInfo());

    for (const FHitResult& HitResult : InHitResults)
    {
        if (APawn* HitPawn = Cast<APawn>(HitResult.GetActor()))
        {
            if (UWarriorFunctionLibrary::IsTargetPawnHostile(OwningPawn, HitPawn))
            {
                FGameplayEventData EventData;
                EventData.Instigator = OwningPawn;
                EventData.Target = HitPawn;

                FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(HitPawn, InSpecHandle);

                if (ActiveGameplayEffectHandle.WasSuccessfullyApplied())
                {
                    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitPawn, WarriorGameplayTags::Shared_Event_HitReact, EventData);
                }
            }
        }
    }
}

TArray<AWarriorEnemyCharacter*> UWarriorGameplayAbility::GetEnemyCharactersFromHitResults(
    const FGameplayEffectSpecHandle& InSpecHandle,
    const TArray<FHitResult>& InHitResults,
    UParticleSystem* InIceVFX,
    FName InAttachSocketName)
{
    TArray<AWarriorEnemyCharacter*> FoundEnemies;

    if (InHitResults.IsEmpty())
    {
        return FoundEnemies;
    }

    APawn* OwningPawn = CastChecked<APawn>(GetAvatarActorFromActorInfo());

    for (const FHitResult& HitResult : InHitResults)
    {
        AActor* HitActor = HitResult.GetActor();
        if (!HitActor)
        {
            continue;
        }

        AWarriorEnemyCharacter* EnemyChar = Cast<AWarriorEnemyCharacter>(HitActor);
        if (!EnemyChar)
        {
            continue;
        }

        if (!UWarriorFunctionLibrary::IsTargetPawnHostile(OwningPawn, EnemyChar))
        {
            continue;
        }

        FoundEnemies.Add(EnemyChar);
        FrozenEnemies.AddUnique(EnemyChar);
        EnemyChar->bIsFrozen = true;

        // --- FREEZE AI ---
        if (AAIController* AIC = Cast<AAIController>(EnemyChar->GetController()))
        {
            if (AWarriorAIController* WarriorAIC = Cast<AWarriorAIController>(AIC))
            {
                WarriorAIC->FreezeAI();
            }
            else
            {
                AIC->StopMovement();
            }
        }

        // --- FREEZE VFX: ATTACHED TO MESH ---
        if (InIceVFX && EnemyChar->GetMesh())
        {
            USkeletalMeshComponent* EnemyMesh = EnemyChar->GetMesh();

            // Try common socket names, fallback to root
            FName AttachSocket = NAME_None;
            TArray<FName> SocketNames = { FName("spine_01"), FName("Spine"), FName("pelvis"), FName("Pelvis"), FName("root") };

            for (const FName& Socket : SocketNames)
            {
                if (EnemyMesh->DoesSocketExist(Socket))
                {
                    AttachSocket = Socket;
                    break;
                }
            }

            UParticleSystemComponent* VFXComp = UGameplayStatics::SpawnEmitterAttached(
                InIceVFX,
                EnemyMesh,
                AttachSocket,
                FVector::ZeroVector,
                FRotator::ZeroRotator,
                EAttachLocation::SnapToTarget,
                true  // bAutoDestroy
            );

            if (VFXComp)
            {
                EnemyChar->AddFreezeVFX(VFXComp);

                UE_LOG(LogTemp, Log, TEXT("FrostGiant: Attached ice VFX to %s on socket %s"),
                    *EnemyChar->GetName(), *AttachSocket.ToString());
            }
        }

        UE_LOG(LogTemp, Log, TEXT("FrostGiant: Frozen enemy %s"), *EnemyChar->GetName());
    }

    return FoundEnemies;
}

void UWarriorGameplayAbility::UnfreezeAllTrackedEnemies()
{
    UE_LOG(LogTemp, Warning, TEXT("UnfreezeAllTrackedEnemies: Unfreezing %d enemies"), FrozenEnemies.Num());

    for (AWarriorEnemyCharacter* EnemyChar : FrozenEnemies)
    {
        if (!IsValid(EnemyChar))
        {
            continue;
        }

        if (AAIController* AIC = Cast<AAIController>(EnemyChar->GetController()))
        {
            if (AWarriorAIController* WarriorAIC = Cast<AWarriorAIController>(AIC))
            {
                WarriorAIC->UnfreezeAI();  // This now also clears VFX
            }
        }
    }

    FrozenEnemies.Empty();

    if (UnfreezeTimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(UnfreezeTimerHandle);
    }
}

void UWarriorGameplayAbility::AttachIceVFXToEnemy(
    AWarriorEnemyCharacter* InEnemy,
    UParticleSystem* InIceVFX,
    FName InAttachSocketName)  // <-- passed from BP
{
    if (!InEnemy || !InIceVFX)
    {
        return;
    }

    USkeletalMeshComponent* EnemyMesh = InEnemy->GetMesh();
    if (!EnemyMesh)
    {
        return;
    }

    // Use the socket name you passed from BP
    FName AttachSocket = InAttachSocketName;

    if (!EnemyMesh->DoesSocketExist(AttachSocket))
    {
        UE_LOG(LogTemp, Warning, TEXT("FrostGiant: Socket %s not found on %s"),
            *AttachSocket.ToString(), *InEnemy->GetName());
        return;
    }

    UParticleSystemComponent* VFXComp = UGameplayStatics::SpawnEmitterAttached(
        InIceVFX,
        EnemyMesh,
        AttachSocket,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::SnapToTarget,
        true
    );

    if (VFXComp)
    {
        InEnemy->AddFreezeVFX(VFXComp);
    }
}





