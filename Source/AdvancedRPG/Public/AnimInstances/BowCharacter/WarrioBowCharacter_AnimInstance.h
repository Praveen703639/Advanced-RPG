// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarrioCharacterAnimInstance.h"
#include "WarrioBowCharacter_AnimInstance.generated.h"

class AWarriorBowCharacter;

/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UWarrioBowCharacter_AnimInstance : public UWarrioCharacterAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:

	UPROPERTY()
	AWarriorBowCharacter* OwningBowCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bIsFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	FRotator AimRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	FRotator VelocityDeltaRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float YawOffset;


	
};
