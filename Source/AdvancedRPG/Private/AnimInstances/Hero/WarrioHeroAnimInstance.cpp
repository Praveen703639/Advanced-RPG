// pravin's patashala all Rights Reserved 

#include "AnimInstances/Hero/WarrioHeroAnimInstance.h"
#include "Characters/WarriorHeroCharacter.h"

void UWarrioHeroAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (OwningCharacter)
	{
		OwningHeroCharacter = Cast<AWarriorHeroCharacter>(OwningCharacter);
	}

}

void UWarrioHeroAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (!OwningHeroCharacter)
	{
		return;
	}
	if (bHasAcceleration)
	{
		IdleElapsedTime = 0.f;
		bShouldEnterRelaxstate = false;

	}
	else
	{
		IdleElapsedTime += DeltaSeconds;
		bShouldEnterRelaxstate = IdleElapsedTime >= EnterRelaxStateThreshold;
	}
}
