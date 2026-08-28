#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "UAT_CircleRhythm.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWolfHitEnemy, AActor*, HitEnemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCircleRhythmComplete);

UCLASS()
class ADVANCEDRPG_API UAT_CircleRhythm : public UAbilityTask
{
    GENERATED_BODY()

public:
    UAT_CircleRhythm();

    UPROPERTY(BlueprintAssignable)
    FOnWolfHitEnemy OnWolfHitEnemy;

    UPROPERTY(BlueprintAssignable)
    FOnCircleRhythmComplete OnCircleRhythmComplete;

    UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Ability|Tasks")
    static UAT_CircleRhythm* CircleRhythm(
        UGameplayAbility* OwningAbility,
        TSubclassOf<AActor> InWolfActorClass,
        int32 InNumWolves,
        float InDuration,
        float InWolfOrbitRadius,
        float InTickInterval,
        float InBoxTraceExtent);

    virtual void Activate() override;
    virtual void ExternalCancel() override;
    virtual void OnDestroy(bool AbilityEnded) override;

protected:
    UPROPERTY()
    TSubclassOf<AActor> WolfActorClass;

    UPROPERTY()
    TArray<AActor*> SpawnedWolves;

    UPROPERTY()
    TArray<float> WolfAngles;

    float Duration;
    float WolfOrbitRadius;
    float TickInterval;
    float BoxTraceExtent;
    int32 NumWolves;

    FTimerHandle OrbitTimer;
    FTimerHandle DamageTickTimer;
    FTimerHandle DurationTimer;

    void StartWolfOrbit();
    void SpawnWolves();
    void UpdateWolfOrbits();
    void TickDamage();
    void PerformBoxTraceForWolf(AActor* Wolf);
    void CleanupAndEnd();
};