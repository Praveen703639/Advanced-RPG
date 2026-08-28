// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WarriorCloneAIController.generated.h"

class UAISenseConfig_Sight;
class UAIPerceptionComponent;
class UBlackboardComponent;
class AAWarriorIceClone;

/**
 * 
 */


UCLASS()
class ADVANCEDRPG_API AWarriorCloneAIController : public AAIController
{
	GENERATED_BODY()

public:
	AWarriorCloneAIController(const FObjectInitializer& ObjectInitializer);

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

protected:
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> AISenseConfig_Sight;

	UPROPERTY()
	TObjectPtr<UAIPerceptionComponent> EnemyPerceptionComponent;

	UFUNCTION()
	 virtual void OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
};
