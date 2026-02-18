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

#include "WarriorDebugHelper.h"

AWarriorHeroCharacter::AWarriorHeroCharacter()
{
    // 1. Physical setup
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // 2. Control Rotation Logic (Cleaned up duplicates)
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false; // Fixed duplicate Pitch

    // 3. Camera Setup
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength = 200.0f;
    CameraBoom->SocketOffset = FVector(0.f, 55.f, 65.f);
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // 4. Movement Tuning
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
}



void AWarriorHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Ensure the Data Asset is assigned (usually in Blueprint)
    checkf(InputConfigUDataAsset, TEXT("Input Config is null! Forget to assign a valid Data Asset?"));

    ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

    if (Subsystem)
    {
        // Use the Mapping Context from your Data Asset
        Subsystem->AddMappingContext(InputConfigUDataAsset->DefaultMappingContext, 0);
    }

    UWarriorInputComponent* WarriorInput = CastChecked<UWarriorInputComponent>(PlayerInputComponent);

    // Binding Move and Look using Gameplay Tags
    WarriorInput->BindNativeInputAction(InputConfigUDataAsset, WarriorGameplayTags::Input_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
    WarriorInput->BindNativeInputAction(InputConfigUDataAsset, WarriorGameplayTags::Input_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
}

void AWarriorHeroCharacter::PossessedBy(AController* NewController)
{
    if (!CharacterStartUpData.IsNull())
    {
        if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
        {
			LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);
        }
    }
}

void AWarriorHeroCharacter::Input_Move(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();
    const FRotator ControlRotation = Controller->GetControlRotation();
    const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

    // Forward/Backward
    if (MovementVector.Y != 0.f)
    {
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(ForwardDirection, MovementVector.Y);
    }

    // Left/Right
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
        AddControllerYawInput(LookAxisVector.X);
    }

    if (LookAxisVector.Y != 0.f)
    {
        AddControllerPitchInput(LookAxisVector.Y);
    }
}