// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarrioCharacterAnimInstance.h"
#include "WarrioHeroAnimInstance.generated.h"
class AWarriorHeroCharacter;

/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UWarrioHeroAnimInstance : public UWarrioCharacterAnimInstance
{
	GENERATED_BODY()

protected:
	
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Referenxes")
	AWarriorHeroCharacter* OwningHeroCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bShouldEnterRelaxstate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float EnterRelaxStateThreshold = 5.f;

	float IdleElapsedTime;
};


