// pravin's patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "GameplayTagContainer.h"
#include "WarriorHeroCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UDataAsset_inputConfig;
class UHeroCombatComponent;
struct FInputActionValue;

/**
 * AWarriorHeroCharacter
 * 
 * The main playable hero character class responsible for player control, input handling, and combat.
 * 
 * Key Features:
 * - Third-person camera with spring arm positioning
 * - Enhanced Input System integration with dynamic input context switching
 * - Ability System Component for ability management and execution
 * - Weapon equipment with dynamic animation layers and input remapping
 * - Combat component for weapon and ability management
 * 
 * Input Architecture:
 * - Base Input (Priority 0): Default movement and look inputs, set in SetupPlayerInputComponent
 * - Weapon Input (Priority 1): Weapon-specific inputs added dynamically when weapon is equipped
 * - This allows seamless input context switching without losing base input
 * 
 * Initialization Flow:
 * 1. Constructor: Setup components (camera, combat component)
 * 2. PossessedBy: Load and grant startup abilities from DataAsset
 * 3. SetupPlayerInputComponent: Bind base inputs (move, look, abilities)
 * 4. Weapon Equipped: AddWeaponInputMappingContext adds weapon-specific inputs
 * 
 * Dependencies:
 * - UWarriorAbilitySystemComponent (inherited from base)
 * - UHeroCombatComponent (for weapon management)
 * - UDataAsset_inputConfig (configuration for all inputs)
 */
UCLASS()
class ADVANCEDRPG_API AWarriorHeroCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()

public:

	AWarriorHeroCharacter();

protected:

	/** 
	 * Called to bind functionality to input.
	 * Sets up base input bindings (move, look, abilities) with priority 0.
	 * Weapon inputs are added separately with priority 1 in AddWeaponInputMappingContext.
	 */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** 
	 * Called when possessed by controller (Server Only).
	 * Loads and grants startup abilities to the character's ability system.
	 */
	virtual void PossessedBy(AController* NewController) override;

private:

	/* =======================
	 *        Components
	 * ======================= */

	/** Camera boom positioning the camera behind the character at an offset for better visibility. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera attached to the camera boom, providing third-person perspective. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Combat component that manages weapons, abilities, and combat-related logic. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UHeroCombatComponent* HeroCombatComponent;

	/* =======================
	 *        Input
	 * ======================= */

	/** 
	 * Input configuration data asset containing all input actions and mappings.
	 * Must be assigned in Blueprint or will assert at runtime.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UDataAsset_inputConfig* InputConfigUDataAsset;

	/**
	 * Handles character movement input based on input magnitude and controller rotation.
	 * Applies movement in forward and right directions separately.
	 */
	void Input_Move(const FInputActionValue& Value);

	/**
	 * Handles camera look input, rotating the character and camera based on input.
	 * Applies yaw rotation to character and pitch rotation to camera for freelook.
	 */
	void Input_Look(const FInputActionValue& Value);

	/**
	 * Called when an ability input is pressed.
	 * Forwards the input tag to the ability system for ability activation.
	 */
	void Input_AbilityInputPressed(const FInputActionValue& Value, FGameplayTag InInputTag);

	/**
	 * Called when an ability input is released.
	 * Forwards the input tag to the ability system for ability deactivation.
	 */
	void Input_AbilityInputReleased(const FInputActionValue& Value, FGameplayTag InInputTag);

public:

	/**
	 * Gets the hero combat component.
	 * @return Pointer to the UHeroCombatComponent managing weapons and combat
	 */
	FORCEINLINE UHeroCombatComponent* GetHeroCombatComponent() const
	{
		return HeroCombatComponent;
	}

	/**
	 * Safely adds a weapon input mapping context to the enhanced input subsystem.
	 * 
	 * This function is called when a weapon is equipped to apply weapon-specific inputs.
	 * It includes defensive null checks for controller, local player, and subsystem.
	 * The weapon context is added with priority 1, allowing it to override base inputs
	 * without affecting the base input context (priority 0).
	 * 
	 * Design: This is a public Blueprint-callable function because weapons are often
	 * equipped through Blueprint abilities (e.g., GA_Hero_EquipAxe), and this ensures
	 * they can safely access input system features without direct subsystem access.
	 * 
	 * @param WeaponMappingContext The input mapping context to apply (weapon-specific inputs)
	 * 
	 * Safety Guarantees:
	 * - Returns safely if context is nullptr
	 * - Returns safely if player controller doesn't exist (e.g., in server-only context)
	 * - Returns safely if local player isn't initialized yet
	 * - Returns safely if subsystem doesn't exist
	 * 
	 * @see SetupPlayerInputComponent for base input setup
	 */
	UFUNCTION(BlueprintCallable, Category = "Warrior|Input")
	void AddWeaponInputMappingContext(UInputMappingContext* WeaponMappingContext);

	/**
	 * Safely removes a weapon input mapping context from the enhanced input subsystem.
	 * 
	 * This function is called when a weapon is unequipped to remove its input bindings.
	 * It includes defensive null checks for controller, local player, and subsystem.
	 * Removes the weapon context cleanly without affecting the base input context.
	 * 
	 * Companion Function: Called after AddWeaponInputMappingContext when unequipping.
	 * Ensures weapon inputs don't persist after the weapon is no longer active.
	 * 
	 * @param WeaponMappingContext The input mapping context to remove (weapon-specific inputs)
	 * 
	 * Safety Guarantees:
	 * - Returns safely if context is nullptr
	 * - Returns safely if player controller doesn't exist
	 * - Returns safely if local player isn't initialized
	 * - Returns safely if subsystem doesn't exist
	 * 
	 * @see AddWeaponInputMappingContext for adding weapon inputs
	 */
	UFUNCTION(BlueprintCallable, Category = "Warrior|Input")
	void RemoveWeaponInputMappingContext(UInputMappingContext* WeaponMappingContext);
};
