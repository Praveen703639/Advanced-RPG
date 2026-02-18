// pravin's  patashala all Rights Reserved 


#include "AnimInstances/WarrioBaseAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/WarriorBaseCharacter.h"


void UWarrioBaseAnimInstance::NativeInitializeAnimation()
{


    OwningCharacter = Cast<AWarriorBaseCharacter>(TryGetPawnOwner());

    if (OwningCharacter)
    {
        OwningMovementComponent = OwningCharacter->GetCharacterMovement();
    }
}

void UWarrioBaseAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
    if (!OwningCharacter || !OwningMovementComponent)
    {
        return;
    }


    GroundSpeed = OwningCharacter->GetVelocity().Size2D();

    bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;
};