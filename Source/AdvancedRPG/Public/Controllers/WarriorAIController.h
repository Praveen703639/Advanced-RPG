// pravin's patashala all Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WarriorAIController.generated.h"

class UAISenseConfig_Sight;
class UAIPerceptionComponent;
class UBlackboardComponent;
class AAWarriorIceClone;

UCLASS()
class ADVANCEDRPG_API AWarriorAIController : public AAIController
{
    GENERATED_BODY()

public:
    AWarriorAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // IGenericTeamAgentInterface
    virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(1); }
    virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

    UFUNCTION()
    void ForceTargetClone(AAWarriorIceClone* Clone);

    UFUNCTION()
    void ClearCloneTarget();

    UFUNCTION(BlueprintPure)
    bool HasCloneTarget() const;

    UPROPERTY(EditDefaultsOnly, Category = "AI|Crowd")
    bool bShouldEnableDetourCrowdAvoidance = true;

    UPROPERTY(EditDefaultsOnly, Category = "AI|Crowd")
    int32 DetourCrowdAvoidanceQuality = 3;

    UPROPERTY(EditDefaultsOnly, Category = "AI|Crowd")
    float CollisionQueryRange = 500.f;

    /** Freeze this AI — stop BT, zero velocity, pause anim. Call from ability. */
    UFUNCTION(BlueprintCallable, Category = "Warrior|AI|Freeze")
    void FreezeAI();

    /** Unfreeze this AI — resume BT, re-enable movement. */
    UFUNCTION(BlueprintCallable, Category = "Warrior|AI|Freeze")
    void UnfreezeAI();

    /** Returns true if this AI is currently frozen. */
    UFUNCTION(BlueprintPure, Category = "Warrior|AI|Freeze")
    bool IsAIFrozen() const { return bIsFrozen; }


    virtual void BeginPlay() override;

    UPROPERTY()
    TObjectPtr<UAISenseConfig_Sight> AISenseConfig_Sight;

    UPROPERTY()
    TObjectPtr<UAIPerceptionComponent> EnemyPerceptionComponent;

    UPROPERTY()
    TWeakObjectPtr<AAWarriorIceClone> CurrentCloneTarget;

    UPROPERTY()
    TWeakObjectPtr<AActor> OriginalHeroTarget;

    UFUNCTION()
    void OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);


    UPROPERTY()
    bool bIsFrozen = false;

    /** Cached movement mode before freeze, to restore on unfreeze. */
    TEnumAsByte<EMovementMode> CachedMovementMode = MOVE_Walking;
};