// pravin's  patashala all Rights Reserved 


#include "Components/Combat/EnemyCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "warriorGameplayTags.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorDebugHelper.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Components/CapsuleComponent.h"





void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (OverlappedActors.Contains(HitActor))
	{
		return;
	}

	OverlappedActors.AddUnique(HitActor);
	//To DO:: Implement Block Checks 
	bool bIsValidBlock = false;


	bool bIsPlayerBlocking = UWarriorFunctionLibrary::NativeDoesActorHaveTag(HitActor, WarriorGameplayTags::Player_Status_Blocking);


	bool bisMyAttackUnblockable = UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetOwningPawn(),WarriorGameplayTags::Enemy_Status_UnBlockable);


	if (bIsPlayerBlocking && !bisMyAttackUnblockable)
	{
        // Check if the block is valid (e.g., correct timing, correct weapon type, etc.)
		// Capture the return value so bIsValidBlock reflects the actual result
		bIsValidBlock = UWarriorFunctionLibrary::IsValidBlock(GetOwningPawn(), HitActor);

	}


	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = HitActor;



	if (bIsValidBlock)
	{
		//Handle successful block (e.g., play block animation, reduce damage, etc.)

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitActor,
			WarriorGameplayTags::Player_Event_SuccessfulBlock,
			EventData
		);
	}
	else
	{
		// Handle successful hit (e.g., apply damage, play hit animation, etc.)
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			GetOwningPawn(),
			WarriorGameplayTags::Shared_Event_MeeleHit,
			EventData
		);

	}


}

void UEnemyCombatComponent::ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	AWarriorEnemyCharacter* OwningEnemyCharacter = GetOwningPawn<AWarriorEnemyCharacter>();

	check(OwningEnemyCharacter);

	UCapsuleComponent* LeftHandCapsuleComponent = OwningEnemyCharacter->GetLeftHandCollisionCapsule();
	UCapsuleComponent* RightHandCapsuleComponent = OwningEnemyCharacter->GetRightHandCollisionCapsule();

	check(LeftHandCapsuleComponent && RightHandCapsuleComponent);

	switch (ToggleDamageType)
	{
	
	case EToggleDamageType::RightLeg:
		break;

	case EToggleDamageType::LeftLeg:
		break;

	case EToggleDamageType::RightHand:
		RightHandCapsuleComponent->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);

		break;

	case EToggleDamageType::LeftHand:
		LeftHandCapsuleComponent->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
		break;

	default:
		break;

	}
	if (!bShouldEnable)
	{
		OverlappedActors.Empty();
	}

}
