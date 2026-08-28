// pravin's  patashala all Rights Reserved 


#include "Characters/WarriorEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/Combat/EnemyCombatComponent.h"
#include "Engine/AssetManager.h"
#include "DataAssets/StartUpData/DataAsset_EnemyStartUpData.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Components/UI/EnemyUIComponent.h"
#include "Components/WidgetComponent.h"
#include "Widgets/InGameWidgets/WarriorWidgetBase.h"
#include "Components/CapsuleComponent.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorBaseGameMode.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

#include "WarriorDebugHelper.h"

AWarriorEnemyCharacter::AWarriorEnemyCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;


	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 180.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;

	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>(TEXT("EnemyCombatComponent"));

    EnemyUIComponent = CreateDefaultSubobject<UEnemyUIComponent>(TEXT("EnemyUIComponent"));

	EnemyHealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyHealthWidgetComponent"));
	EnemyHealthWidgetComponent->SetupAttachment(GetMesh());

	LeftHandCollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>("LeftHandCollisionCapsule");
	LeftHandCollisionCapsule->SetupAttachment(GetMesh());
	LeftHandCollisionCapsule->SetCapsuleRadius(8.f);
	LeftHandCollisionCapsule->SetCapsuleHalfHeight(25.f);
	LeftHandCollisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCollisionCapsule->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);



	RightHandCollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>("RightHandCollisionCapsule");
	RightHandCollisionCapsule->SetupAttachment(GetMesh());
	RightHandCollisionCapsule->SetCapsuleRadius(8.f);
	RightHandCollisionCapsule->SetCapsuleHalfHeight(25.f);
	RightHandCollisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollisionCapsule->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);


	RightLegCollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>("RightLegCollisionCapsule");
	RightLegCollisionCapsule->SetupAttachment(GetMesh());
	RightLegCollisionCapsule->SetCapsuleRadius(8.f);
	RightLegCollisionCapsule->SetCapsuleHalfHeight(30.f);
	RightLegCollisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightLegCollisionCapsule->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);


	LeftLegCollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>("LeftLegCollisionCapsule");
	LeftLegCollisionCapsule->SetupAttachment(GetMesh());
	LeftLegCollisionCapsule->SetCapsuleRadius(8.f);
	LeftLegCollisionCapsule->SetCapsuleHalfHeight(30.f);
	LeftLegCollisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftLegCollisionCapsule->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);


    // Make the boss mesh block camera probes so the spring arm pushes in
// instead of letting the camera clip inside the boss during attacks.
    GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
}

UPawnCombatComponent* AWarriorEnemyCharacter::GetPawnCombatComponent() const
{
    return EnemyCombatComponent;
}

UPawnUIComponents* AWarriorEnemyCharacter::GetPawnUIComponents() const
{
    return EnemyUIComponent;
}


UEnemyUIComponent* AWarriorEnemyCharacter::GetEnemyUIComponent() const
{
    return EnemyUIComponent;
}

#if WITH_EDITOR
void AWarriorEnemyCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);


	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, LeftHandCollisionCapsuleAttachBoneName))
	{
		LeftHandCollisionCapsule->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftHandCollisionCapsuleAttachBoneName);
	}


	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, RightHandCollisionCapsuleAttachBoneName))
	{
		RightHandCollisionCapsule->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandCollisionCapsuleAttachBoneName);
	}

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, RightLegCollisionCapsuleAttachBoneName))
	{
		RightLegCollisionCapsule->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightLegCollisionCapsuleAttachBoneName);
	}

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, LeftLegCollisionCapsuleAttachBoneName))
	{
		LeftLegCollisionCapsule->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftLegCollisionCapsuleAttachBoneName);
	}

}
#endif

void AWarriorEnemyCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    Init_StartUpData();
}

void AWarriorEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();


    if (UWarriorWidgetBase* HealthWidget = Cast<UWarriorWidgetBase>(EnemyHealthWidgetComponent->GetUserWidgetObject()))
    {
        HealthWidget->InitEnemyCreatedWidget(this);
    }
}

void AWarriorEnemyCharacter::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (APawn* HitPawn = Cast<APawn>(OtherActor))
    {
        if (UWarriorFunctionLibrary::IsTargetPawnHostile(this, HitPawn))
        {
            EnemyCombatComponent->OnHitTargetActor(HitPawn);
        }
    }
}

void AWarriorEnemyCharacter::Init_StartUpData()
{
   
    if (CharacterStartUpData.IsNull())
    {
        return;
    }

    int32 AbilityApplyLevel = 1;

    if (AWarriorBaseGameMode* BaseGameMode = GetWorld()->GetAuthGameMode<AWarriorBaseGameMode>())
    {
        switch (BaseGameMode->GetCurrentGameDifficulty())
        {
        case EWarriorGameDifficulty::Easy:
            AbilityApplyLevel = 1;
            break;

        case EWarriorGameDifficulty::Normal:
            AbilityApplyLevel = 2;
            break;

        case EWarriorGameDifficulty::Hard:
            AbilityApplyLevel = 3;
            break;

        case EWarriorGameDifficulty::VeryHard:
            AbilityApplyLevel = 4;
            break;

        default:
            break;
        }
    }

    UAssetManager::GetStreamableManager().RequestAsyncLoad(
        CharacterStartUpData.ToSoftObjectPath(),
        FStreamableDelegate::CreateLambda(
            [this, AbilityApplyLevel]()
            {
                
                if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.Get())
                {
                   
                    if (WarriorAbilitySystemComponent)
                    {
                        LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent,AbilityApplyLevel);

                        
                    }

                     
                }
            }
        )
    );
}

void AWarriorEnemyCharacter::AddFreezeVFX(UParticleSystemComponent* VFXComponent)
{
    if (IsValid(VFXComponent))
    {
        ActiveFreezeVFX.Add(VFXComponent);
    }
}


void AWarriorEnemyCharacter::ClearFreezeVFX()
{
    for (UParticleSystemComponent* VFX : ActiveFreezeVFX)
    {
        if (IsValid(VFX))
        {
            VFX->DestroyComponent();
        }
    }
    ActiveFreezeVFX.Empty();
}

