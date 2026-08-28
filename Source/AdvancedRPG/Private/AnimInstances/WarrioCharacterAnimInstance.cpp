// pravin's  patashala all Rights Reserved 


#include "AnimInstances/WarrioCharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/WarriorBaseCharacter.h"
#include "KismetAnimationLibrary.h"

void UWarrioCharacterAnimInstance::NativeInitializeAnimation()
{
    OwningCharacter = Cast<AWarriorBaseCharacter>(TryGetPawnOwner());

    if (OwningCharacter)
    {
        OwningMovementComponent = OwningCharacter->GetCharacterMovement();
    }
}

void UWarrioCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
    if (!OwningCharacter || !OwningMovementComponent)
    {
        return;
    }


    GroundSpeed = OwningCharacter->GetVelocity().Size2D();

    bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;

    LocoMotionDirection = UKismetAnimationLibrary::CalculateDirection(OwningCharacter->GetVelocity(), OwningCharacter->GetActorRotation());
}
