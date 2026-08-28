// pravin's patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "GameplayTagContainer.h"
#include "WarriorBowCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UDataAsset_inputConfig;
class UPawnUIComponents;
class UHeroUIComponent;
class UBowCombatComponent;
struct FInputActionValue;
class AWarriorArrow;
/**
 *
 */
UCLASS()
class ADVANCEDRPG_API AWarriorBowCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()
public:
	AWarriorBowCharacter();

	/**
	 * Called when possessed — initialises ASC and spawns bow weapons (arrows).
	 */
	virtual void PossessedBy(AController* NewController) override;

	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	virtual UPawnUIComponents* GetPawnUIComponents() const override;
	virtual UHeroUIComponent* GetHeroUIComponent() const override;

	/** Input configuration data asset containing all input actions and mappings.
	 * Made public so it is visible and editable on both Blueprint class defaults and placed instances.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UDataAsset_inputConfig* InputConfigUDataAsset;

	/** Default arrow class to spawn when BowStartUpWeapons array is empty */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bow|Weapon")
	TSubclassOf<AWarriorArrow> DefaultArrowClass;

	/** Spawns and registers the default bow arrow */
	UFUNCTION(BlueprintCallable, Category = "Bow|Weapon")
	void SpawnDefaultBowArrow();

	// Get the bow combat component
	FORCEINLINE UBowCombatComponent* GetBowCombatComponent() const { return BowCombatComponent; }

protected:
	/**
	 * Called to bind functionality to input.
	 * Sets up base input bindings (move, look, abilities) with priority 0.
	 * Weapon inputs are added separately with priority 1 in AddWeaponInputMappingContext.
	 */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	/* =======================
	 *        Components
	 * ======================= */

	 /** Camera boom positioning the camera behind the character at an offset for better visibility. */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* CameraBoom;

	/** Follow camera attached to the camera boom, providing third-person perspective. */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* FollowCamera;

	/** Bow-specific combat component for managing arrows and bow-specific combat logic */
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	UBowCombatComponent* BowCombatComponent;

	/** Player UI component (health, rage, weapon icon) for the bow character HUD. */
	UPROPERTY(VisibleAnywhere, Category = "UI")
	UHeroUIComponent* BowUIComponent;

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

	void Input_Jump(const FInputActionValue& Value);

	void Input_AbilityInputPressed(const FInputActionValue& Value, FGameplayTag InInputTag);
	void Input_AbilityInputReleased(const FInputActionValue& Value, FGameplayTag InInputTag);
};