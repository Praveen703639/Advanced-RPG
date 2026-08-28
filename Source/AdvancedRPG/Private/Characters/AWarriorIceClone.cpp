// pravin's patashala all Rights Reserved

#include "Characters/AWarriorIceClone.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Controllers/WarriorAIController.h"
#include "GenericTeamAgentInterface.h"
#include "Animation/AnimInstance.h"           // <-- FIX: Missing include
#include "Materials/MaterialInstanceDynamic.h" // <-- FIX: Missing include
#include "Materials/MaterialInterface.h"       // <-- FIX: Missing include

// Sets default values
AAWarriorIceClone::AAWarriorIceClone()
{
    PrimaryActorTick.bCanEverTick = true;

    // Disable movement — clone is stationary
    GetCharacterMovement()->DisableMovement();
    GetCharacterMovement()->SetMovementMode(MOVE_None);

    // Block camera like real hero
    GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);

    // Capsule setup for aggro detection
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);

    // AI Perception source — THIS makes AI "see" the clone
    UAIPerceptionStimuliSourceComponent* PerceptionSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionSource"));
    PerceptionSource->bAutoRegister = true;
    PerceptionSource->RegisterForSense(UAISense_Sight::StaticClass());
    PerceptionSource->RegisterWithPerceptionSystem();

    // Damage handling — bind in BeginPlay, NOT constructor (delegate binding in ctor can be problematic)
}

// Called when the game starts or when spawned
void AAWarriorIceClone::BeginPlay()
{
    Super::BeginPlay();

    // Bind damage delegate here, not in constructor
   // ✅ Alternative — lambda approach with AddUObject
    // ✅ CORRECT — use the proper delegate access
    Super::OnTakeAnyDamage.AddDynamic(this, &AAWarriorIceClone::OnTakeAnyDamage);

    SetupForAggro();
}

// Called every frame
void AAWarriorIceClone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsDestroying)
    {
        UpdateVisuals(DeltaTime);
        return;
    }

    ElapsedTime += DeltaTime;

    if (ElapsedTime >= MaxDuration)
    {
        FadeOutAndDestroy();
    }
}

void AAWarriorIceClone::SetupForAggro()
{
    // Clone needs to be perceived as hostile by enemy AI.
    // Your IsTargetPawnHostile checks controller team IDs.
    // Since clone has no controller, we need to give it one with the same team as hero.

    if (!SourceHero.IsValid())
    {
        return;
    }

    AController* HeroController = SourceHero->GetController();
    if (!HeroController)
    {
        return;
    }

    IGenericTeamAgentInterface* HeroTeamAgent = Cast<IGenericTeamAgentInterface>(HeroController);
    if (!HeroTeamAgent)
    {
        return;
    }

    // Option 1: Spawn a dummy AI controller and set same team
    // Option 2: Make the clone itself implement IGenericTeamAgentInterface
    // For now, let's spawn a minimal AI controller for the clone

    // NOTE: If you want enemies to properly detect this as hostile,
    // the clone MUST have a controller with a team ID that enemies consider hostile.
    // Since Hero = Team 0, Enemy AI = Team 1, enemies see Team 0 as hostile.
    // So clone needs to be on Team 0.

    // Simplest approach: The clone itself implements IGenericTeamAgentInterface
    // But since ACharacter doesn't implement it by default, we need a controller.

    // For now, this is a placeholder — see note below in "IMPORTANT" section
}

void AAWarriorIceClone::InitializeClone(ACharacter* InSourceHero, float InDuration)
{
    if (!InSourceHero)
    {
        UE_LOG(LogTemp, Warning, TEXT("AAWarriorIceClone::InitializeClone: InSourceHero is null!"));
        return;
    }

    SourceHero = InSourceHero;
    MaxDuration = InDuration;
    ElapsedTime = 0.f;
    bIsDestroying = false;

    USkeletalMeshComponent* HeroMesh = SourceHero->GetMesh();
    USkeletalMeshComponent* CloneMesh = GetMesh();

    if (!HeroMesh || !CloneMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("AAWarriorIceClone::InitializeClone: Mesh missing!"));
        return;
    }

    // Copy hero's skeletal mesh
    CloneMesh->SetSkeletalMesh(HeroMesh->GetSkeletalMeshAsset());

    // Copy transform (location + rotation relative to capsule)
    CloneMesh->SetRelativeTransform(HeroMesh->GetRelativeTransform());

    // Freeze animation — use single node, no animation playback
    CloneMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);

    // Copy current pose if possible
    if (UAnimInstance* HeroAnimInst = HeroMesh->GetAnimInstance())
    {
        // Optional: Snapshot pose — this is advanced, skip for now
        // CloneMesh->SetAnimInstanceClass(HeroAnimInst->GetClass());
    }

    // Setup ice visuals
    SetupVisualsFromHero();
}

void AAWarriorIceClone::SetupVisualsFromHero()
{
    USkeletalMeshComponent* CloneMesh = GetMesh();
    if (!CloneMesh)
    {
        return;
    }

    // Create dynamic materials for all mesh slots
    const int32 NumMaterials = CloneMesh->GetNumMaterials();
    DynamicMaterials.Empty(NumMaterials);

    for (int32 i = 0; i < NumMaterials; ++i)
    {
        UMaterialInstanceDynamic* DynMat = CloneMesh->CreateAndSetMaterialInstanceDynamic(i);
        if (DynMat)
        {
            DynamicMaterials.Add(DynMat);
        }
    }

    // NOTE: Set ice material parameters in Blueprint or via a Gameplay Effect
    // Don't use LoadObject here — bad practice. Use Blueprint defaults instead.
}

void AAWarriorIceClone::UpdateVisuals(float DeltaTime)
{
    if (FadeOutDuration <= 0.f)
    {
        Destroy();
        return;
    }

    ElapsedTime += DeltaTime;

    float FadeAlpha = 1.f - (ElapsedTime / FadeOutDuration);
    FadeAlpha = FMath::Clamp(FadeAlpha, 0.f, 1.f);

    for (UMaterialInstanceDynamic* DynMat : DynamicMaterials)
    {
        if (DynMat)
        {
            DynMat->SetScalarParameterValue(IceMaterialOpacityParam, FadeAlpha);
        }
    }

    if (FadeAlpha <= 0.f)
    {
        Destroy();
    }
}

void AAWarriorIceClone::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    if (bIsDestroying)
    {
        return;
    }

    CloneHealth -= Damage;

    if (CloneHealth <= 0.f)
    {
        FadeOutAndDestroy();
    }
}

void AAWarriorIceClone::FadeOutAndDestroy()
{
    if (bIsDestroying)
    {
        return;
    }

    bIsDestroying = true;
    ElapsedTime = 0.f; // Reset timer for fade-out phase

    // Disable collision so no more damage
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAWarriorIceClone::DestroyClone()
{
    FadeOutAndDestroy();
}

void AAWarriorIceClone::Destroyed()
{
    // Notify that clone is being destroyed
    if (SourceHero.IsValid())
    {
        // TODO: Broadcast delegate or send gameplay event to hero
    }

    Super::Destroyed();
}

float AAWarriorIceClone::GetRemainingDuration() const
{
    if (bIsDestroying)
    {
        return 0.f;
    }
    return FMath::Max(0.f, MaxDuration - ElapsedTime);
}

ACharacter* AAWarriorIceClone::GetSourceHero() const
{
    return SourceHero.Get();
}