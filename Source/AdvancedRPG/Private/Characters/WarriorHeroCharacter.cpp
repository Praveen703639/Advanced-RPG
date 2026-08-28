// pravin's patashala all Rights Reserved 

#include "Characters/WarriorHeroCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DataAssets/Input/DataAsset_inputConfig.h"
#include "EnhancedInputSubsystems.h"
#include "Components/Input/WarriorInputComponent.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Controllers/WarriorHeroController.h"
#include "Components/UI/HeroUIComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorBaseGameMode.h"


#include "WarriorDebugHelper.h"

AWarriorHeroCharacter::AWarriorHeroCharacter()
{
    // 1. Physical setup
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // 2. Control Rotation Logic
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // 3. Camera Setup
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength = 200.0f;
    CameraBoom->SocketOffset = FVector(0.f, 55.f, 65.f);
    CameraBoom->bUsePawnControlRotation = true;

    // Prevent the camera from clipping into world geometry and enemy meshes by enabling
    // the spring arm collision test. The probe will shorten the arm when hitting blockers.
    CameraBoom->bDoCollisionTest = true;
    CameraBoom->ProbeSize = 12.0f;

    // Use ECC_Camera channel so we can control exactly what blocks the camera
    // independently from visibility/line-of-sight traces.
    CameraBoom->ProbeChannel = ECC_Camera;

    // Optional: Smooth the camera movement when the arm length changes due to collision
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 3.0f;
    CameraBoom->bEnableCameraRotationLag = true;
    CameraBoom->CameraRotationLagSpeed = 10.0f;

    FollowCamera = CreateDefaultSubobject <UCameraComponent >(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));
    HeroUIComponent = CreateDefaultSubobject<UHeroUIComponent>(TEXT("HeroUIComponent"));

    // 4. Movement Tuning
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
}

UPawnCombatComponent* AWarriorHeroCharacter::GetPawnCombatComponent() const
{
    return HeroCombatComponent;
}

UPawnUIComponents* AWarriorHeroCharacter::GetPawnUIComponents() const
{
    return HeroUIComponent;
}


UHeroUIComponent* AWarriorHeroCharacter::GetHeroUIComponent() const
{
    return  HeroUIComponent;
}


void AWarriorHeroCharacter::PossessedBy(AController* NewController)
{
    

    // Super (WarriorBaseCharacter) already calls InitAbilityActorInfo and GiveToAbilitySystemComponent.
    // We only need to add the hero-specific weapon setup that the base class doesn't know about.
    if (!CharacterStartUpData.IsNull())
    {
        if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
        {
           
            {
                int32 AbilityApplyLevel = 1;

                if (AWarriorBaseGameMode* BaseGameMode = GetWorld()->GetAuthGameMode<AWarriorBaseGameMode>())
                {
                    switch (BaseGameMode->GetCurrentGameDifficulty())
                    {
                    case EWarriorGameDifficulty::Easy:
                        AbilityApplyLevel = 4;
                        Debug::Print(TEXT("Current Difficulty: Easy"));
                        break;

                    case EWarriorGameDifficulty::Normal:
                        AbilityApplyLevel = 3;
                        Debug::Print(TEXT("Current Difficulty: Normal"));
                        break;

                    case EWarriorGameDifficulty::Hard:
                        AbilityApplyLevel = 2;
                        Debug::Print(TEXT("Current Difficulty: Hard"));
                        break;

                    case EWarriorGameDifficulty::VeryHard:
                        AbilityApplyLevel = 1;
                        Debug::Print(TEXT("Current Difficulty: Very Hard"));
                        break;

                    default:
                        break;
                    }
                }

                LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent, AbilityApplyLevel);
            }
        }
    }
}

void AWarriorHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    checkf(InputConfigUDataAsset, TEXT("Input Config is null! Forget to assign a valid Data Asset?"));

    APlayerController* PlayerController = GetController<APlayerController>();
    if (!PlayerController) return;

    ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
    if (!LocalPlayer) return;

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

    if (Subsystem && InputConfigUDataAsset->DefaultMappingContext)
    {
        Subsystem->AddMappingContext(InputConfigUDataAsset->DefaultMappingContext, 0);
    }

    UWarriorInputComponent* WarriorInput = CastChecked<UWarriorInputComponent>(PlayerInputComponent);

    WarriorInput->BindNativeInputAction(InputConfigUDataAsset, WarriorGameplayTags::Input_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
    WarriorInput->BindNativeInputAction(InputConfigUDataAsset, WarriorGameplayTags::Input_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);

    WarriorInput->BindNativeInputAction(InputConfigUDataAsset, WarriorGameplayTags::Input_SwitchTarget, ETriggerEvent::Triggered, this, &ThisClass::Input_SwitchTargetTriggered);
    WarriorInput->BindNativeInputAction(InputConfigUDataAsset, WarriorGameplayTags::Input_SwitchTarget, ETriggerEvent::Completed, this, &ThisClass::Input_SwitchTargetLockCompleted);

    WarriorInput->BindAbilityInputAction(InputConfigUDataAsset, this, &AWarriorHeroCharacter::Input_AbilityInputPressed, &AWarriorHeroCharacter::Input_AbilityInputReleased);
    WarriorInput->BindNativeInputAction(InputConfigUDataAsset, WarriorGameplayTags::Input_PickUp_Stones, ETriggerEvent::Started, this, &ThisClass::Input_PickUpStonesStarted);
}

void AWarriorHeroCharacter::Input_Move(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();
    const FRotator ControlRotation = Controller->GetControlRotation();
    const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

    if (MovementVector.Y != 0.f)
    {
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(ForwardDirection, MovementVector.Y);
    }

    if (MovementVector.X != 0.f)
    {
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void AWarriorHeroCharacter::Input_Look(const FInputActionValue& Value)
{
    const FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (LookAxisVector.X != 0.f)
    {
        //  Negate X so mouse right = look right
        AddControllerYawInput(-LookAxisVector.X);
    }

    if (LookAxisVector.Y != 0.f)
    {
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void AWarriorHeroCharacter::Input_AbilityInputPressed(const FInputActionValue& Value, FGameplayTag InInputTag)
{
    if (WarriorAbilitySystemComponent)
    {
        WarriorAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
    }
}

void AWarriorHeroCharacter::Input_AbilityInputReleased(const FInputActionValue& Value, FGameplayTag InInputTag)
{
    if (WarriorAbilitySystemComponent)
    {
        WarriorAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
    }
}

void AWarriorHeroCharacter::Input_SwitchTargetTriggered(const FInputActionValue& Value)
{

    SwitchDirection = Value.Get<FVector2D>();

}

void AWarriorHeroCharacter::Input_SwitchTargetLockCompleted(const FInputActionValue& Value)
{
    FGameplayEventData Data;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
        this,
        SwitchDirection.X > 0 ? WarriorGameplayTags::Player_Event_SwitchTarget_Right : WarriorGameplayTags::Player_Event_SwitchTarget_Left,
        Data
    );



}

void AWarriorHeroCharacter::Input_PickUpStonesStarted(const FInputActionValue& InputActionValue)
{
    FGameplayEventData Data;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
        this,
        WarriorGameplayTags::Player_Event_ConsumeStones,
        Data
    );
}

void AWarriorHeroCharacter::AddWeaponInputMappingContext(UInputMappingContext* WeaponMappingContext)
{
    if (!WeaponMappingContext) return;

    APlayerController* PlayerController = GetController<APlayerController>();
    if (!PlayerController) return;

    ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
    if (!LocalPlayer) return;

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
    if (Subsystem)
    {
        Subsystem->AddMappingContext(WeaponMappingContext, 1);
    }
}

void AWarriorHeroCharacter::RemoveWeaponInputMappingContext(UInputMappingContext* WeaponMappingContext)
{
    if (!WeaponMappingContext) return;

    APlayerController* PlayerController = GetController<APlayerController>();
    if (!PlayerController) return;

    ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
    if (!LocalPlayer) return;

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
    if (Subsystem)
    {
        Subsystem->RemoveMappingContext(WeaponMappingContext);
    }
}



