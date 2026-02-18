// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "WarriorGameplayAbility.generated.h"

UENUM()
enum class EWarriorAbilityActivationPolicy : uint8
{
	OnTriggered,
	OnGiven,

};

/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UWarriorGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
		UPROPERTY(EditDefaultsOnly,Category = "WarriorAbility")
		EWarriorAbilityActivationPolicy ActivationPolicy = EWarriorAbilityActivationPolicy::OnTriggered;

		//~ Begin UGameplayAbility Interface.
		virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
		//~ End UGameplayAbility Interface

		//~ Begin UGameplayAbility Interface.
		virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
		//~ End UGameplayAbility Interface

};
