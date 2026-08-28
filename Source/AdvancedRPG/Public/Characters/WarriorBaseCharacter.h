// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/PawnCombatInterface.h"
#include "Interfaces/PawnUIInterface.h"
#include "Items/Weapons/WarriorWeaponBase.h"

#include "WarriorBaseCharacter.generated.h"

class UWarriorAbilitySystemComponent;
class UWarriorAttributeSet;
class UDataAsset_StartUpDataBase;
class UMotionWarpingComponent;
class UWarriorWeaponBase;




UCLASS()
class ADVANCEDRPG_API AWarriorBaseCharacter : public ACharacter, public IAbilitySystemInterface, public IPawnCombatInterface, public IPawnUIInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWarriorBaseCharacter();

protected:
 UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "AbilitySystem")
	UWarriorAbilitySystemComponent* WarriorAbilitySystemComponent;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
UWarriorAttributeSet* WarriorAttributeSet;

	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface

	//~ Begin IAbilitySystemInterface Interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface Interface

	//~ Begin IPawnUIInterface Interface
	virtual UPawnUIComponents* GetPawnUIComponents() const override;
	//~ End IPawnUIInterface Interface

	virtual  UPawnCombatComponent* GetPawnCombatComponent() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	TSoftObjectPtr<UDataAsset_StartUpDataBase> CharacterStartUpData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionWarping")
	UMotionWarpingComponent* MotionWarpingComponent;



public:
	FORCEINLINE UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponent() const { return WarriorAbilitySystemComponent; }
	FORCEINLINE UWarriorAttributeSet* GetWarriorAttributeSet() const { return WarriorAttributeSet; }
	FORCEINLINE TSoftObjectPtr<UDataAsset_StartUpDataBase> GetCharacterStartUpData() const { return CharacterStartUpData; }
	

};    
