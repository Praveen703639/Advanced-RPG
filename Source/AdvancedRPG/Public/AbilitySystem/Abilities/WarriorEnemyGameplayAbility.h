// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "WarriorEnemyGameplayAbility.generated.h"
class AWarriorEnemyCharacter;
class UEnemyCombatComponent;

/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UWarriorEnemyGameplayAbility : public UWarriorGameplayAbility
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	AWarriorEnemyCharacter* GetWarriorEnemyCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")

	UEnemyCombatComponent* GetEnemyCombatComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	FGameplayEffectSpecHandle MakeEnemyDamageEffectSpceHandle(TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat);


private:
	TWeakObjectPtr<AWarriorEnemyCharacter> CachedWarriorEnemyCharacter;
	
	
};
