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
 * Hero Playable Character
 */
UCLASS()
class ADVANCEDRPG_API AWarriorHeroCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()

public:

	AWarriorHeroCharacter();

protected:

	/** Called to bind functionality to input */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** Called when possessed by controller (Server Only) */
	virtual void PossessedBy(AController* NewController) override;

private:

	/* =======================
	 *        Components
	 * ======================= */

	 /** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Combat Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UHeroCombatComponent* HeroCombatComponent;

	/* =======================
	 *        Input
	 * ======================= */

	 /** Input configuration data asset */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UDataAsset_inputConfig* InputConfigUDataAsset;

	/** Movement Input */
	void Input_Move(const FInputActionValue& Value);

	/** Look Input */
	void Input_Look(const FInputActionValue& Value);

	/** Ability Input Pressed */
	void Input_AbilityInputPressed(const FInputActionValue& Value, FGameplayTag InInputTag);

	/** Ability Input Released */
	void Input_AbilityInputReleased(const FInputActionValue& Value, FGameplayTag InInputTag);

public:

	/** Getter for Combat Component */
	FORCEINLINE UHeroCombatComponent* GetHeroCombatComponent() const
	{
		return HeroCombatComponent;
	}
};
