// pravin's  patashala all Rights Reserved 


#include "AnimInstances/BowCharacter/WarrioBowCharacter_AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/WarriorBowCharacter.h"



void UWarrioBowCharacter_AnimInstance::NativeInitializeAnimation()
{
 Super::NativeInitializeAnimation();

	// Cache a typed pointer to the owning bow character and ensure movement component is valid
	OwningBowCharacter = Cast<AWarriorBowCharacter>(TryGetPawnOwner());
	if (OwningBowCharacter && !OwningMovementComponent)
	{
		OwningMovementComponent = OwningBowCharacter->GetCharacterMovement();
	}
}

void UWarrioBowCharacter_AnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
   Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	// Defensive checks: ensure pointers are valid before accessing
	if (!OwningBowCharacter)
	{
		OwningBowCharacter = Cast<AWarriorBowCharacter>(TryGetPawnOwner());
	}

	if (!OwningMovementComponent && OwningBowCharacter)
	{
		OwningMovementComponent = OwningBowCharacter->GetCharacterMovement();
	}

	if (!OwningBowCharacter || !OwningMovementComponent)
	{
		return;
	}

	bIsFalling = OwningMovementComponent->IsFalling();
	AimRotation = OwningBowCharacter->GetBaseAimRotation();
	Velocity = OwningBowCharacter->GetVelocity();

        // Compute rotator representing the velocity direction (MakeRotFromX equivalent)
		FRotator VelocityRotator = Velocity.IsNearlyZero() ? FRotator::ZeroRotator : Velocity.Rotation();

        // Compute delta between velocity direction rotator and base aim rotation
		VelocityDeltaRotator = (VelocityRotator - AimRotation).GetNormalized();

		// Break rotator and extract yaw component for yaw offset
		YawOffset = VelocityDeltaRotator.Yaw;
}
