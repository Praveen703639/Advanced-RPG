// pravin's patashala all Rights Reserved

#include "AbilitySystem/Abilities/HeroGameplayAbility_CloneIce.h"
#include "AbilitySystem/AbilityTasks/UAT_SpawnIceClone.h"
#include "Characters/WarriorHeroCharacter.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "WarriorGameplayTags.h"

UHeroGameplayAbility_CloneIce::UHeroGameplayAbility_CloneIce()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	CloneActiveTag = FGameplayTag::RequestGameplayTag(FName("Status.Hero.CloneActive"));
}

void UHeroGameplayAbility_CloneIce::ActivateAbility(
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

	AWarriorHeroCharacter* Hero = GetWarriorHeroCharacterFromActorInfo();
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
	if (UWarriorAbilitySystemComponent* ASC = GetWarriorAbilitySystemComponentFromActorInfo())
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

	ActiveSpawnTask->OnCloneSpawned.AddDynamic(this, &UHeroGameplayAbility_CloneIce::OnCloneSpawned);
	ActiveSpawnTask->OnCloneDestroyed.AddDynamic(this, &UHeroGameplayAbility_CloneIce::OnCloneDestroyed);
	ActiveSpawnTask->ReadyForActivation();
}

void UHeroGameplayAbility_CloneIce::OnCloneSpawned(AActor* SpawnedClone)
{
	// Override in Blueprint or add C++ reaction logic here
	// (e.g. camera shake, extra VFX, audio cue)
}

void UHeroGameplayAbility_CloneIce::OnCloneDestroyed()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UHeroGameplayAbility_CloneIce::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (bCloneTagApplied)
	{
		if (UWarriorAbilitySystemComponent* ASC = GetWarriorAbilitySystemComponentFromActorInfo())
		{
			ASC->RemoveLooseGameplayTag(CloneActiveTag);
		}
		bCloneTagApplied = false;
	}

	ActiveSpawnTask = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}