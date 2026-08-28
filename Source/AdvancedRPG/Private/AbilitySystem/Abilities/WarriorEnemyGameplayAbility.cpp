// pravin's  patashala all Rights Reserved 


#include "AbilitySystem/Abilities/WarriorEnemyGameplayAbility.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "warriorGameplayTags.h"

AWarriorEnemyCharacter* UWarriorEnemyGameplayAbility::GetWarriorEnemyCharacterFromActorInfo() 
{
    if (!CachedWarriorEnemyCharacter.IsValid())
    {
        // Remove the parentheses after AvatarActor
        CachedWarriorEnemyCharacter = Cast<AWarriorEnemyCharacter>(CurrentActorInfo->AvatarActor);
    }
    return CachedWarriorEnemyCharacter.IsValid() ? CachedWarriorEnemyCharacter.Get() : nullptr;
}

UEnemyCombatComponent* UWarriorEnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo() 
{
	return GetWarriorEnemyCharacterFromActorInfo()->GetEnemyCombatComponent();
}

FGameplayEffectSpecHandle UWarriorEnemyGameplayAbility::MakeEnemyDamageEffectSpceHandle(TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat)
{
   
    check(EffectClass);
    FGameplayEffectContextHandle ContextHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeEffectContext();
    ContextHandle.SetAbility(this);
    ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
    ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

    FGameplayEffectSpecHandle EffectSpecHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
        EffectClass,
        GetAbilityLevel(),
        ContextHandle
    );

	EffectSpecHandle.Data->SetSetByCallerMagnitude(
        WarriorGameplayTags::Shared_SetByCaller_BaseDamage,
        InDamageScalableFloat.GetValueAtLevel(GetAbilityLevel())
    );
	return EffectSpecHandle;

    
}



