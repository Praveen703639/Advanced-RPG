// pravin's patashala all Rights Reserved

#include "Characters/WarriorBowCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Components/Combat/BowCombatComponent.h"
#include "DataAssets/StartUpData/DataAsset_BowStartUpData.h"
#include "Items/Weapons/WarriorArrow.h"

#include "DataAssets/Input/DataAsset_inputConfig.h"
#include "Components/Input/WarriorInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/UI/PawnUIComponents.h"
#include "Components/UI/HeroUIComponent.h"
#include "WarriorGameplayTags.h"
#include "Controllers/WarriorBowController.h"

AWarriorBowCharacter::AWarriorBowCharacter()
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

    CameraBoom->TargetArmLength = 300.0f;
    CameraBoom->SocketOffset = FVector(0.f, 55.f, 65.f);
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bDoCollisionTest = true;
    CameraBoom->ProbeSize = 18.0f;
    CameraBoom->ProbeChannel = ECC_Camera;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 6.0f;
    CameraBoom->bEnableCameraRotationLag = true;
    CameraBoom->CameraRotationLagSpeed = 10.0f;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // Bow-specific combat component
    BowCombatComponent = CreateDefaultSubobject<UBowCombatComponent>(TEXT("BowCombatComponent"));
    BowUIComponent = CreateDefaultSubobject<UHeroUIComponent>(TEXT("BowUIComponent"));

    // 4. Movement Tuning
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
}

UPawnCombatComponent* AWarriorBowCharacter::GetPawnCombatComponent() const
{
    return BowCombatComponent;
}

UPawnUIComponents* AWarriorBowCharacter::GetPawnUIComponents() const
{
    return BowUIComponent;
}

UHeroUIComponent* AWarriorBowCharacter::GetHeroUIComponent() const
{
    return BowUIComponent;
}

// ============================================
// Spawn default arrow when BowStartUpWeapons array is empty
// The bow mesh has Arrow_Socket built-in, but combat system needs an AWarriorArrow actor
// ============================================
void AWarriorBowCharacter::SpawnDefaultBowArrow()
{
    if (!BowCombatComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("WarriorBowCharacter::SpawnDefaultBowArrow: BowCombatComponent is NULL!"));
        return;
    }

    if (!DefaultArrowClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("WarriorBowCharacter::SpawnDefaultBowArrow: DefaultArrowClass not set in Blueprint!"));
        return;
    }

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = this;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AWarriorArrow* Arrow = GetWorld()->SpawnActor<AWarriorArrow>(
        DefaultArrowClass,
        GetActorLocation(),
        GetActorRotation(),
        Params
    );

    if (!Arrow)
    {
        UE_LOG(LogTemp, Error, TEXT("WarriorBowCharacter::SpawnDefaultBowArrow: Failed to spawn arrow!"));
        return;
    }

    // Attach to Arrow_Socket on the bow mesh
    Arrow->AttachToComponent(
        GetMesh(),
        FAttachmentTransformRules::SnapToTargetIncludingScale,
        FName("Arrow_Socket")
    );

    // Set ownership for GAS
    Arrow->SetOwner(this);
    Arrow->SetInstigator(this);

    // ============================================
    // 🔥 CRITICAL FIX: Use the SAME tag as your Data Asset: Player.Weapon.Arrow
    // NOT Weapon.Bow — that was the tag mismatch bug!
    // ============================================
    FGameplayTag ArrowTag = FGameplayTag::RequestGameplayTag(FName("Player.Weapon.Arrow"));
    BowCombatComponent->RegisterSpawnedWeapon(ArrowTag, Arrow, true);

    UE_LOG(LogTemp, Warning, TEXT("WarriorBowCharacter: Spawned arrow '%s' on Arrow_Socket, registered with tag '%s'"),
        *Arrow->GetName(), *ArrowTag.ToString());
}

void AWarriorBowCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (WarriorAbilitySystemComponent && !CharacterStartUpData.IsNull())
    {
        if (UDataAsset_BowStartUpData* BowData = Cast<UDataAsset_BowStartUpData>(CharacterStartUpData.LoadSynchronous()))
        {
            // Give bow abilities
            BowData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent, 1);

            // Spawn weapons from data asset if configured
            BowData->GiveBowWeaponsToCombatComponent(this, BowCombatComponent, 1);

            // ============================================
            // 🔥 CRITICAL FIX: If data asset didn't spawn an arrow, spawn default
            // This ensures bow ALWAYS has an arrow regardless of config
            // ============================================
            if (!BowCombatComponent || !BowCombatComponent->GetBowCurrentEquippedArrow())
            {
                UE_LOG(LogTemp, Warning, TEXT("WarriorBowCharacter: BowStartUpWeapons didn't equip arrow, spawning default..."));
                SpawnDefaultBowArrow();
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("WarriorBowCharacter: Failed to cast CharacterStartUpData to DataAsset_BowStartUpData!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WarriorBowCharacter: Cannot setup weapons - ASC=%s, StartUpDataNull=%s"),
            WarriorAbilitySystemComponent ? TEXT("Valid") : TEXT("NULL"),
            CharacterStartUpData.IsNull() ? TEXT("Yes") : TEXT("No"));
    }
}

void AWarriorBowCharacter::Input_Move(const FInputActionValue& Value)
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

void AWarriorBowCharacter::Input_Look(const FInputActionValue& Value)
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

void AWarriorBowCharacter::Input_Jump(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        Jump();
    }
    else
    {
        StopJumping();
    }
}

void AWarriorBowCharacter::Input_AbilityInputPressed(const FInputActionValue& Value, FGameplayTag InInputTag)
{
    if (WarriorAbilitySystemComponent)
    {
        WarriorAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
    }
}

void AWarriorBowCharacter::Input_AbilityInputReleased(const FInputActionValue& Value, FGameplayTag InInputTag)
{
    if (WarriorAbilitySystemComponent)
    {
        WarriorAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
    }
}

void AWarriorBowCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
    WarriorInput->BindNativeInputAction(InputConfigUDataAsset, WarriorGameplayTags::Input_Jump, ETriggerEvent::Started, this, &ThisClass::Input_Jump);
    WarriorInput->BindNativeInputAction(InputConfigUDataAsset, WarriorGameplayTags::Input_Jump, ETriggerEvent::Completed, this, &ThisClass::Input_Jump);
    WarriorInput->BindAbilityInputAction(InputConfigUDataAsset, this, &ThisClass::Input_AbilityInputPressed, &ThisClass::Input_AbilityInputReleased);

    if (!InputConfigUDataAsset->FindNativeInputActionByTag(WarriorGameplayTags::Input_Jump))
    {
        UE_LOG(LogTemp, Warning, TEXT("[WarriorBowCharacter] Jump InputAction not found in InputConfig for %s"), *GetName());
    }

    if (!InputConfigUDataAsset->DefaultMappingContext)
    {
        UE_LOG(LogTemp, Warning, TEXT("[WarriorBowCharacter] DefaultMappingContext is null on InputConfig for %s"), *GetName());
    }
}