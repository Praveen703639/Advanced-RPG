// pravin's patashala all Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGameplayAbility_CloneIce.generated.h"

class UParticleSystem;
class AAWarriorIceClone;
class UUAT_SpawnIceClone;

UCLASS()
class ADVANCEDRPG_API UHeroGameplayAbility_CloneIce : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()

public:
	UHeroGameplayAbility_CloneIce();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frozen Simulacrum")
	TSubclassOf<AAWarriorIceClone> IceCloneClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frozen Simulacrum")
	FGameplayTag SpawnCloneEventTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frozen Simulacrum")
	float CloneDuration = 7.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frozen Simulacrum")
	float LeapDistance = 700.f;

	// Cascade particle spawned at clone location
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frozen Simulacrum|Effects")
	UParticleSystem* IceParticleEffect;

	// Cascade particle attached to hero (like Aurora ice effect)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frozen Simulacrum|Effects")
	UParticleSystem* ParticleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frozen Simulacrum|Effects")
	FName ParticleAttachSocket = FName("pelvis");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frozen Simulacrum")
	FGameplayTag CloneActiveTag;

	UFUNCTION()
	void OnCloneSpawned(AActor* SpawnedClone);

	UFUNCTION()
	void OnCloneDestroyed();

private:
	UPROPERTY()
	UUAT_SpawnIceClone* ActiveSpawnTask = nullptr;

	bool bCloneTagApplied = false;
};