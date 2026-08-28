// pravin's patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "WarriorTypes/WarriorStructTypes.h"

#include "WarriorAbilitySystemComponent.generated.h"

UCLASS()
class ADVANCEDRPG_API UWarriorAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void OnAbilityInputPressed(const FGameplayTag& InInputTagToGive);
	void OnAbilityInputReleased(const FGameplayTag& InInputTagToGive);

	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	void GrantHeroWeaponAbilities(
		const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities,
		const TArray<FWarriorHeroWeaponSpecialAbilitySet>& InSpecialWeaponAbilities,
		int32 ApplyLevel,
		TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles);

	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	void RemoveGrantedHeroWeaponAbilities(UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);

	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	bool TryActivateAbilityByInputTag(FGameplayTag InInputTagToGive);

	UPROPERTY(BlueprintReadOnly, Category = "Warrior|Ability")
	bool bStartupAbilitiesGiven = false;

	

};