// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"

#include "WarrioBaseAnimInstance.generated.h"


/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UWarrioBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool DoesOwnerHaveTag(FGameplayTag TagToCheck) const;






};
	

