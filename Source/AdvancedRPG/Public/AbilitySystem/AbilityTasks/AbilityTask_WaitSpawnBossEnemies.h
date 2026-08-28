// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameplayTagContainer.h"

#include "AbilityTask_WaitSpawnBossEnemies.generated.h"

class AWarriorEnemyCharacter;
class UGameplayAbility;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnBossEnemies, const TArray<AWarriorEnemyCharacter*>&, SpawnedEnemies);

/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UAbilityTask_WaitSpawnBossEnemies : public UAbilityTask
{
	GENERATED_BODY()



public:

    UFUNCTION(BlueprintCallable, Category = "Warrior|AbilityTasks", meta = (DisplayName = "Wait Gameplay Event And Spawn Boss Enemies", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", NumToSpawn = "1", RandomSpawnRadius = "200"))
	static UAbilityTask_WaitSpawnBossEnemies* WaitSpawnBossEnemies(
		UGameplayAbility* OwningAbility,
		FGameplayTag EventTag,
        const TArray<TSoftClassPtr<AWarriorEnemyCharacter>>& ClassesToSpawn,
		int32 NumToSpawn,
		const FVector& SpawnOrigin,
		float RandomSpawnRadius

	);

	//~ Begin UGameplayTask Interface
	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;
	//~ End UGameplayTask Interface

	UPROPERTY(BlueprintAssignable)
	FOnSpawnBossEnemies OnSpawnFinished;

	UPROPERTY(BlueprintAssignable)
	FOnSpawnBossEnemies DidNotSpawn;
	
    // Cached parameters
private:
	FGameplayTag CachedEventTag;
	TArray<TSoftClassPtr<AWarriorEnemyCharacter>> CachedClassesToSpawn;
	int32 CachedNumToSpawn;
	FVector CachedSpawnOrigin;
	float CachedRandomSpawnRadius;
	FDelegateHandle DelegateHandle;

	void OnGameplayEventReceived(const FGameplayEventData* InPayload);

	void OnEnemyClassesLoaded();
};
