// pravin's  patashala all Rights Reserved 


#include "AnimInstances/WarrioBaseAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/WarriorBaseCharacter.h"
#include "WarriorFunctionLibrary.h"

bool UWarrioBaseAnimInstance::DoesOwnerHaveTag(FGameplayTag TagToCheck) const
{
	if (APawn* OwningPawn = TryGetPawnOwner())
	{
		return UWarriorFunctionLibrary::NativeDoesActorHaveTag(OwningPawn, TagToCheck);

	}
	return false;
}
