// pravin's patashala all Rights Reserved

#include "Controllers/WarriorAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/AWarriorIceClone.h"
#include "GenericTeamAgentInterface.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


#include "WarriorDebugHelper.h"

AWarriorAIController::AWarriorAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
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
    EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &AWarriorAIController::OnEnemyPerceptionUpdated);

    SetGenericTeamId(FGenericTeamId(1));
}

ETeamAttitude::Type AWarriorAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
    const APawn* PawnToCheck = Cast<const APawn>(&Other);
    if (!PawnToCheck)
    {
        return ETeamAttitude::Neutral;
    }

    const IGenericTeamAgentInterface* OtherTeamAgent = nullptr;

    // Try controller first
    if (PawnToCheck->GetController())
    {
        OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(PawnToCheck->GetController());
    }

    // If no controller, check pawn itself (for ice clones that implement IGenericTeamAgentInterface)
    if (!OtherTeamAgent)
    {
        OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(PawnToCheck);
    }

    const IGenericTeamAgentInterface* MyTeamAgent = Cast<const IGenericTeamAgentInterface>(this);
    if (!MyTeamAgent || !OtherTeamAgent)
    {
        return ETeamAttitude::Neutral;
    }

    // Different team IDs = hostile
    if (OtherTeamAgent->GetGenericTeamId() != MyTeamAgent->GetGenericTeamId())
    {
        return ETeamAttitude::Hostile;
    }

    return ETeamAttitude::Friendly;
}

void AWarriorAIController::FreezeAI()
{

    if (bIsFrozen)
    {
        return; // Already frozen
    }

    bIsFrozen = true;

    // 1. Stop Behavior Tree (prevents new MoveTo tasks)
    if (UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(BrainComponent))
    {
        BTComp->PauseLogic("FrozenByAbility");
    }

    // 2. Stop any active path following / MoveTo
    StopMovement();

    // 3. Zero velocity and disable movement on character
    if (APawn* ControlledPawn = GetPawn())
    {
        if (ACharacter* ControlledChar = Cast<ACharacter>(ControlledPawn))
        {
            if (UCharacterMovementComponent* MoveComp = ControlledChar->GetCharacterMovement())
            {
                CachedMovementMode = MoveComp->MovementMode;
                MoveComp->Velocity = FVector::ZeroVector;
                MoveComp->StopMovementImmediately();
                MoveComp->DisableMovement();
            }

            // 4. Pause animation
            if (USkeletalMeshComponent* Mesh = ControlledChar->GetMesh())
            {
                if (UAnimInstance* AnimInst = Mesh->GetAnimInstance())
                {
                    AnimInst->Montage_Stop(0.0f);
                }
                Mesh->bPauseAnims = true;
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("AI %s: FROZEN"), *GetName());
}

void AWarriorAIController::UnfreezeAI()
{
    if (!bIsFrozen)
    {
        return;
    }

    bIsFrozen = false;

    // 1. Resume Behavior Tree
    if (UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(BrainComponent))
    {
        BTComp->ResumeLogic("FrozenByAbility");
    }

    // 2. Re-enable movement
    if (APawn* ControlledPawn = GetPawn())
    {
        if (ACharacter* ControlledChar = Cast<ACharacter>(ControlledPawn))
        {
            if (UCharacterMovementComponent* MoveComp = ControlledChar->GetCharacterMovement())
            {
                MoveComp->SetMovementMode(CachedMovementMode);
            }

            // 3. Resume animation
            if (USkeletalMeshComponent* Mesh = ControlledChar->GetMesh())
            {
                Mesh->bPauseAnims = false;
            }
        }
    }

    // 4. Clear freeze VFX on the enemy character
    if (AWarriorEnemyCharacter* EnemyChar = Cast<AWarriorEnemyCharacter>(GetPawn()))
    {
        EnemyChar->bIsFrozen = false;
        EnemyChar->ClearFreezeVFX();
    }

    UE_LOG(LogTemp, Log, TEXT("AI %s: UNFROZEN"), *GetName());
}

void AWarriorAIController::BeginPlay()
{
    Super::BeginPlay();

    UCrowdFollowingComponent* CrowdComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent());
    if (!CrowdComp)
    {
        return;
    }

    CrowdComp->SetCrowdSimulationState(
        bShouldEnableDetourCrowdAvoidance ? ECrowdSimulationState::Enabled : ECrowdSimulationState::Disabled
    );

    switch (DetourCrowdAvoidanceQuality)
    {
    case 1:  CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Low);    break;
    case 2:  CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Medium); break;
    case 3:  CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);   break;
    case 4:
    case 5:
    case 6:  CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);  break;
    default: break;
    }

    CrowdComp->SetAvoidanceGroup(1);
    CrowdComp->SetGroupsToAvoid(1);
    CrowdComp->SetCrowdCollisionQueryRange(CollisionQueryRange);
}

void AWarriorAIController::OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
    if (!BlackboardComponent)
    {
        return;
    }

    // PRIORITY 1: Check if this is an ice clone
    AAWarriorIceClone* Clone = Cast<AAWarriorIceClone>(Actor);
    if (Clone)
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            // Clone detected! Save current hero target and switch to clone
            if (!OriginalHeroTarget.IsValid())
            {
                OriginalHeroTarget = Cast<AActor>(BlackboardComponent->GetValueAsObject(FName("TargetActor")));
            }

            BlackboardComponent->SetValueAsObject(FName("TargetActor"), Clone);
            CurrentCloneTarget = Clone;

            Debug::Print(TEXT("AI: Switched target to ICE CLONE"), FColor::Cyan);
            return;
        }
        else
        {
            // Lost sight of clone — check if it's destroyed or invalid
            if (CurrentCloneTarget == Clone && (!Clone || Clone->IsPendingKillPending()))
            {
                ClearCloneTarget();
            }
        }
    }

    // PRIORITY 2: Hero detection (only if no clone target)
    if (!CurrentCloneTarget.IsValid())
    {
        if (!BlackboardComponent->GetValueAsObject(FName("TargetActor")))
        {
            if (Stimulus.WasSuccessfullySensed() && Actor)
            {
                BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
                Debug::Print(TEXT("AI: Targeting HERO"), FColor::Green);
            }
        }
    }
}

void AWarriorAIController::ForceTargetClone(AAWarriorIceClone* Clone)
{
    if (!Clone || !GetBlackboardComponent())
    {
        return;
    }

    // Save current target if we haven't already
    if (!OriginalHeroTarget.IsValid())
    {
        OriginalHeroTarget = Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(FName("TargetActor")));
    }

    GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), Clone);
    CurrentCloneTarget = Clone;

    Debug::Print(TEXT("AI: Forced target to ICE CLONE"), FColor::Cyan);
}

void AWarriorAIController::ClearCloneTarget()
{
    if (!GetBlackboardComponent())
    {
        return;
    }

    CurrentCloneTarget = nullptr;

    // Restore original hero target if still valid
    if (OriginalHeroTarget.IsValid() && !OriginalHeroTarget->IsPendingKillPending())
    {
        GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), OriginalHeroTarget.Get());
        Debug::Print(TEXT("AI: Restored target to HERO"), FColor::Green);
    }
    else
    {
        GetBlackboardComponent()->ClearValue(FName("TargetActor"));
        Debug::Print(TEXT("AI: No target"), FColor::Red);
    }

    OriginalHeroTarget = nullptr;
}

bool AWarriorAIController::HasCloneTarget() const
{
    return CurrentCloneTarget.IsValid() && !CurrentCloneTarget->IsPendingKillPending();
}