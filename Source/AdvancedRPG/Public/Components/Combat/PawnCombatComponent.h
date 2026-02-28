// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "GameplayTagContainer.h"

#include "PawnCombatComponent.generated.h"
class AWarriorWeaponBase;
/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UPawnCombatComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	void RegisterSpawndWeapon(FGameplayTag InWeaponTagToRegister, AWarriorWeaponBase* InWeaponToRegister,bool bRegisterAsEquippedWeapon = false);

	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCarriedweaponByTag(FGameplayTag InWeaponTagToGet) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Warrior|Combat")
	FGameplayTag CurrentEquippedWeaponTag;

	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCurrentEquippedWeapon() const;


private:
	TMap<FGameplayTag, AWarriorWeaponBase*> CharacterCarriedWeaponMap;


	
};
