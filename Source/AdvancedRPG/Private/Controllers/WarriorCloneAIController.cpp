// pravin's  patashala all Rights Reserved 


#include "Controllers/WarriorCloneAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GenericTeamAgentInterface.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "WarriorDebugHelper.h"

AWarriorCloneAIController::AWarriorCloneAIController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("Pathfollowing Component"))
{
    AISenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("EnemySenseConfig_Sight"));
    AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true;
    AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = false;
    AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = false;
    AISenseConfig_Sight->SightRadius = 5000.f;
    AISenseConfig_Sight->LoseSightRadius = 0.f;
    AISenseConfig_Sight->PeripheralVisionAngleDegrees = 360.f;

    EnemyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("EnemyPerceptionComponent"));
    EnemyPerceptionComponent->ConfigureSense(*AISenseConfig_Sight);
    EnemyPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());
    EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnEnemyPerceptionUpdated);

	SetGenericTeamId(FGenericTeamId(0));
}

ETeamAttitude::Type AWarriorCloneAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
    const APawn* PawnToCheck = Cast<const APawn>(&Other);
	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(PawnToCheck->GetController());
	if (OtherTeamAgent && GetGenericTeamId() != OtherTeamAgent->GetGenericTeamId())
	{
		return ETeamAttitude::Hostile;
	}
    return ETeamAttitude::Friendly;
}

 void AWarriorCloneAIController::OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
     if (Stimulus.WasSuccessfullySensed() && Actor)
     {
         if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
         {
			 BlackboardComponent->SetValueAsObject(FName("CloneTargetActor"), Actor);
			 Debug::Print(TEXT("Clone AI: Targeting HERO"), FColor::Green);
         }
     }
}
