// pravin's  patashala all Rights Reserved 


#include "AnimInstances/Hero/WarriorAnimLinkedLayer.h"
#include "AnimInstances/Hero/WarrioHeroAnimInstance.h"

UWarrioHeroAnimInstance* UWarriorAnimLinkedLayer::GetHeroAnimInstance() const
{
	return Cast<UWarrioHeroAnimInstance>(GetOwningComponent()->GetAnimInstance());
}
