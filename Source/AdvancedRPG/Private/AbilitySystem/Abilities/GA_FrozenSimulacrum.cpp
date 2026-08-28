// pravin's patashala all Rights Reserved

#include "AbilitySystem/Abilities/GA_FrozenSimulacrum.h"
#include "AbilitySystem/AbilityTasks/UAT_SpawnIceClone.h"
#include "Characters/WarriorHeroCharacter.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystemComponent.h"

UGA_FrozenSimulacrum::UGA_FrozenSimulacrum()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
    CloneActiveTag = FGameplayTag::RequestGameplayTag(FName("Status.Hero.CloneActive"));
}

void UGA_FrozenSimulacrum::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!HasAuthority(&ActivationInfo))
    {
        return;
    }

    if (ActorInfo == nullptr || ActorInfo->AbilitySystemComponent.Get() == nullptr)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    AWarriorHeroCharacter* Hero = Cast<AWarriorHeroCharacter>(ActorInfo->AvatarActor);
    if (!Hero)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    // Apply clone active status tag
    if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
    {
        ASC->AddLooseGameplayTag(CloneActiveTag);
        bCloneTagApplied = true;
    }

    // === Montage is handled in Blueprint ===
    // === Gameplay event is sent from an AnimNotify ===
    // This task just LISTENS for that event tag and spawns the clone when it fires.

    ActiveSpawnTask = UUAT_SpawnIceClone::SpawnIceClone(
        this,
        SpawnCloneEventTag,
        IceCloneClass,
        CloneDuration,
        LeapDistance,
        true,                   // leap on event
        IceParticleEffect,      // Cascade at clone location
        ParticleEffect,         // Cascade attached to hero
        ParticleAttachSocket    // e.g. "pelvis"
    );

    if (!ActiveSpawnTask)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    ActiveSpawnTask->OnCloneSpawned.AddDynamic(this, &UGA_FrozenSimulacrum::OnCloneSpawned);
    ActiveSpawnTask->OnCloneDestroyed.AddDynamic(this, &UGA_FrozenSimulacrum::OnCloneDestroyed);
    ActiveSpawnTask->ReadyForActivation();
}

void UGA_FrozenSimulacrum::OnCloneSpawned(AActor* SpawnedClone)
{
    // Override in Blueprint or add C++ reaction logic here
    // (e.g. camera shake, extra VFX, audio cue)
}

void UGA_FrozenSimulacrum::OnCloneDestroyed()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_FrozenSimulacrum::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    // Always clear the status tag on end, regardless of which path got us here
    // (normal completion, cancel, or early bail-out above)
    if (bCloneTagApplied && ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
    {
        ActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(CloneActiveTag);
        bCloneTagApplied = false;
    }

    ActiveSpawnTask = nullptr;

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}