// WarriorAllKillAbilityTask.h
#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "WarriorAllKillAbilityTask.generated.h"

class AWarriorEnemyCharacter;

// === FIX: Only ONE delegate, with target actor parameter ===
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAllKillComplete, AActor*, InHitTargetActor);

// ===  Delegate for EACH wolf hit ===
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWolfHit, AActor*, InHitTargetActor);

UCLASS()
class ADVANCEDRPG_API UAT_AllKill : public UAbilityTask
{
    GENERATED_BODY()

public:
    // === : Only ONE delegate instance ===
    UPROPERTY(BlueprintAssignable)
    FOnAllKillComplete OnAllKillComplete;

    // === : Wolf hit delegate ===
    UPROPERTY(BlueprintAssignable)
    FOnWolfHit OnWolfHit;

    // === WOLF EYES ===
    UPROPERTY(BlueprintReadWrite)
    int32 NumWolfEyes = 8;

    UPROPERTY(BlueprintReadWrite)
    float WolfEyeSpawnRadius = 400.0f;

    UPROPERTY(BlueprintReadWrite)
    float WolfEyeSpawnHeightMin = 150.0f;

    UPROPERTY(BlueprintReadWrite)
    float WolfEyeSpawnHeightMax = 350.0f;

    UPROPERTY(BlueprintReadWrite)
    float EyeGlowDuration = 0.5f;

    // === WOLF SPAWN ===
    UPROPERTY(BlueprintReadWrite)
    float WolfSpawnInterval = 0.15f;

    UPROPERTY(BlueprintReadWrite)
    float WolfHopSpeed = 1200.0f;

    UPROPERTY(BlueprintReadWrite)
    float WolfHopArcHeight = 200.0f;

    // === TRACE SETTINGS ===
    UPROPERTY(BlueprintReadWrite)
    float TraceRange = 3000.0f;

    // === CHANGED: Increased Z extent to catch air enemies at ~Z 442+ ===
    UPROPERTY(BlueprintReadWrite)
    FVector TraceBoxExtent = FVector(500.0f, 500.0f, 800.0f);

    UPROPERTY(BlueprintReadWrite)
    TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

    // === CASCADE PARTICLES ===
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VFX")
    UParticleSystem* WolfEyeParticle;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VFX")
    UParticleSystem* PortalParticle;

    // === WOLF MESH ===
    UPROPERTY(BlueprintReadWrite)
    TSubclassOf<AActor> WolfSkeletalMeshClass;

    UPROPERTY(BlueprintReadWrite)
    UAnimMontage* WolfHopMontage;

    // === TARGET ===
    UPROPERTY(BlueprintReadWrite)
    TWeakObjectPtr<AActor> TargetActor;

    // === BLUEPRINT GETTERS ===
    UFUNCTION(BlueprintCallable, Category = "AllKill")
    FVector GetTargetLocation() const { return TargetLocation; }

    UFUNCTION(BlueprintCallable, Category = "AllKill")
    int32 GetCurrentWolfIndex() const { return CurrentWolfIndex; }

    // === NODE CREATION ===
    UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
    static UAT_AllKill* AllKill(
        UGameplayAbility* OwningAbility,
        UParticleSystem* InWolfEyeParticle,
        UParticleSystem* InPortalParticle,
        TSubclassOf<AActor> InWolfMesh,
        UAnimMontage* InWolfHopMontage);

    virtual void Activate() override;
    virtual void ExternalCancel() override;
    virtual void OnDestroy(bool AbilityEnded) override;

protected:
    UPROPERTY()
    TArray<UParticleSystemComponent*> SpawnedWolfEyes;

    UPROPERTY()
    TArray<FVector> WolfEyeLocations;

    UPROPERTY()
    TArray<UParticleSystemComponent*> SpawnedPortals;

    UPROPERTY()
    TArray<AActor*> SpawnedWolves;

    UPROPERTY()
    FTimerHandle WolfSpawnTimer;

    UPROPERTY()
    int32 CurrentWolfIndex = 0;

    UPROPERTY()
    FVector TargetLocation;

    UFUNCTION()
    bool FindTargetViaBoxTrace();

    // === CHANGED: Now checks Actor Tag "Boss" too ===
    UFUNCTION()
    bool IsBossCharacter(AActor* Actor);

    UFUNCTION()
    void SpawnWolfEyes();

    UFUNCTION()
    void SpawnNextWolf();

    UFUNCTION()
    void LaunchWolfToTarget(AActor* Wolf, FVector StartLocation);

    UFUNCTION()
    void CleanupAndEnd();

    FVector GetRandomEyeLocation();
};