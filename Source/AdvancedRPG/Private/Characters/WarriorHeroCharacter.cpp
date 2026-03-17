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

    HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));

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

    // Cache the player controller for safe access
    APlayerController* PlayerController = GetController<APlayerController>();
    if (!PlayerController)
    {
        return;
    }

    // Get the local player from the controller
    ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
    if (!LocalPlayer)
    {
        return;
    }

    // Get the enhanced input subsystem for this local player
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

    // Apply the default input mapping context with priority 0 (base priority)
    // Weapon contexts will be added later with priority 1, allowing them to override base inputs
    if (Subsystem && InputConfigUDataAsset->DefaultMappingContext)
    {
        Subsystem->AddMappingContext(InputConfigUDataAsset->DefaultMappingContext, 0);
    }

    // Cast to our custom input component to access our custom binding functions
    UWarriorInputComponent* WarriorInput = CastChecked<UWarriorInputComponent>(PlayerInputComponent);

    // Bind native inputs (move and look) using gameplay tags
    // This allows the ability system to recognize and respond to these inputs
    WarriorInput->BindNativeInputAction(InputConfigUDataAsset, WarriorGameplayTags::Input_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
    WarriorInput->BindNativeInputAction(InputConfigUDataAsset, WarriorGameplayTags::Input_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);

    // Bind ability inputs that forward to the ability system
    WarriorInput->BindAbilityInputAction(InputConfigUDataAsset, this, &AWarriorHeroCharacter::Input_AbilityInputPressed, &AWarriorHeroCharacter::Input_AbilityInputReleased);
}

void AWarriorHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!CharacterStartUpData.IsNull())
	{
		if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
		{
			LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);

			// Cast to hero-specific startup data and spawn weapons if available
			if (UDataAsset_HeroStartUpData* HeroStartUpData = Cast<UDataAsset_HeroStartUpData>(LoadedData))
			{
				HeroStartUpData->GiveWeaponsToCombatComponent(this, HeroCombatComponent);
			}
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

void AWarriorHeroCharacter::Input_AbilityInputPressed(
    const FInputActionValue& Value,
    FGameplayTag InInputTag)
{
    if (WarriorAbilitySystemComponent)
    {
        WarriorAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
    }
}

void AWarriorHeroCharacter::Input_AbilityInputReleased(
    const FInputActionValue& Value,
    FGameplayTag InInputTag)
{
    if (WarriorAbilitySystemComponent)
    {
        WarriorAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
    }
}

void AWarriorHeroCharacter::AddWeaponInputMappingContext(UInputMappingContext* WeaponMappingContext)
{
    // Safety check: ensure the mapping context is valid
    if (!WeaponMappingContext)
    {
        return;
    }

    // Cache the player controller
    APlayerController* PlayerController = GetController<APlayerController>();
    if (!PlayerController)
    {
        // This can happen in server-only contexts or before the controller is assigned
        return;
    }

    // Get the local player from the controller
    ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
    if (!LocalPlayer)
    {
        // Local player not yet initialized (rare but possible during setup)
        return;
    }

    // Get the enhanced input subsystem for this local player
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
    if (Subsystem)
    {
        // Add the weapon's input mapping context with priority 1 (higher than base priority 0)
        // This allows weapon-specific inputs to override base inputs without removing the base context
        Subsystem->AddMappingContext(WeaponMappingContext, 1);
    }
}

void AWarriorHeroCharacter::RemoveWeaponInputMappingContext(UInputMappingContext* WeaponMappingContext)
{
    // Safety check: ensure the mapping context is valid
    if (!WeaponMappingContext)
    {
        return;
    }

    // Cache the player controller
    APlayerController* PlayerController = GetController<APlayerController>();
    if (!PlayerController)
    {
        // This can happen in server-only contexts or when controller is not assigned
        return;
    }

    // Get the local player from the controller
    ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
    if (!LocalPlayer)
    {
        // Local player not yet initialized (rare but possible during setup)
        return;
    }

    // Get the enhanced input subsystem for this local player
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
    if (Subsystem)
    {
        // Remove the weapon's input mapping context
        // This cleans up weapon-specific inputs when the weapon is unequipped
        Subsystem->RemoveMappingContext(WeaponMappingContext);
    }
}


