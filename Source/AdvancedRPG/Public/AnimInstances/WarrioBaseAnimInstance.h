// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WarrioBaseAnimInstance.generated.h"
class AWarriorBaseCharacter;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UWarrioBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds);

protected:

	UPROPERTY()
	AWarriorBaseCharacter* OwningCharacter;

	UPROPERTY()
	UCharacterMovementComponent* OwningMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float GroundSpeed;



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bHasAcceleration;

};
	

