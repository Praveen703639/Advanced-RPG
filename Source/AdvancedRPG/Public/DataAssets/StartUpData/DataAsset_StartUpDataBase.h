// pravin's patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset_StartUpDataBase.generated.h"

class UWarriorGameplayAbility;
class UWarriorAbilitySystemComponent;
class UGameplayEffect;

UCLASS()
class ADVANCEDRPG_API UDataAsset_StartUpDataBase : public UDataAsset
{
	GENERATED_BODY()

public:
	// Main entry function called by the Character
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1);

protected:
	// Improved helper to handle UE 5.6 syntax
	void GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>>& AbilitiesToGive,
		UWarriorAbilitySystemComponent* InASCToGive,
		int32 ApplyLevel = 1);

public:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf<UWarriorGameplayAbility>> ActivateOnGivenAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf<UWarriorGameplayAbility>> ReactiveAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf<UGameplayEffect>> StartUpGameplayEffects;
};