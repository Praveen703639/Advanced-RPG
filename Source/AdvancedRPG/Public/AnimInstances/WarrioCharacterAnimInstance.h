// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarrioBaseAnimInstance.h"

#include "WarrioCharacterAnimInstance.generated.h"
class AWarriorBaseCharacter;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UWarrioCharacterAnimInstance : public UWarrioBaseAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;


protected:

	UPROPERTY()
	AWarriorBaseCharacter* OwningCharacter;

	UPROPERTY()
	UCharacterMovementComponent* OwningMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float GroundSpeed;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bHasAcceleration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float LocoMotionDirection;

};
