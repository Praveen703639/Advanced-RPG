// pravin's patashala all Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "UAT_SpawnIceClone.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCloneSpawned, AActor*, SpawnedClone);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCloneDestroyed);

class AAWarriorIceClone;
class UParticleSystem;

UCLASS()
class ADVANCEDRPG_API UUAT_SpawnIceClone : public UAbilityTask
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnCloneSpawned OnCloneSpawned;

    UPROPERTY(BlueprintAssignable)
    FOnCloneDestroyed OnCloneDestroyed;

    UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Ability|Tasks")
    static UUAT_SpawnIceClone* SpawnIceClone(
        UGameplayAbility* OwningAbility,
        FGameplayTag EventTag,
        TSubclassOf<AAWarriorIceClone> CloneClass,
        float CloneDuration,
        float LeapDistance,
        bool bShouldLeapOnEvent,
        UParticleSystem* InIceParticleEffect,
        UParticleSystem* InParticleEffect,
        FName InParticleAttachSocket = NAME_None
    );

    virtual void Activate() override;
    virtual void ExternalCancel() override;
    virtual void OnDestroy(bool AbilityEnded) override;

protected:
    UPROPERTY()
    FGameplayTag ListenEventTag;

    UPROPERTY()
    TSubclassOf<AAWarriorIceClone> IceCloneClass;

    UPROPERTY()
    TWeakObjectPtr<AAWarriorIceClone> SpawnedClone;

    UPROPERTY()
    UParticleSystem* IceParticleEffect;

    UPROPERTY()
    UParticleSystem* ParticleEffect;

    UPROPERTY()
    FName ParticleAttachSocket;

    float Duration;
    float LeapDist;
    bool bLeapOnEvent;

    FVector CachedOriginalLocation;
    FRotator CachedOriginalRotation;

    FTimerHandle CloneDurationTimerHandle;
    FDelegateHandle EventDelegateHandle;

    // ✅ FIX: Delegate uses const FGameplayEventData* (pointer!)
    
    void OnGameplayEventReceived(const FGameplayTag CallbackTag, const FGameplayEventData* Payload);

    void SpawnTheClone();
    void PerformHeroLeap();
    void DestroySpawnedClone();

    UPROPERTY()
    TWeakObjectPtr<class UParticleSystemComponent> AttachedParticleComponent;

private:
    bool bIsBeingDestroyed = false;

};