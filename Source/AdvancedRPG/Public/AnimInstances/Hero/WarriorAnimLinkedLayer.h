// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarrioBaseAnimInstance.h"
#include "WarriorAnimLinkedLayer.generated.h"
class UWarrioHeroAnimInstance;
/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UWarriorAnimLinkedLayer : public UWarrioBaseAnimInstance
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	UWarrioHeroAnimInstance* GetHeroAnimInstance() const;


	
};
