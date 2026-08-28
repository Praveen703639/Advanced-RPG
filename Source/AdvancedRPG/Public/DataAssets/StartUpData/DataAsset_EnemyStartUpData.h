// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "DataAsset_EnemyStartUpData.generated.h"
class UWarriorEnemyGameplayAbility;
/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UDataAsset_EnemyStartUpData : public UDataAsset_StartUpDataBase
{
	GENERATED_BODY()
public:
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1);

private:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf< UWarriorEnemyGameplayAbility>> EnemyCombatAbilities;
	
};
