// pravin's patashala all Rights Reserved 

#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "GameplayEffect.h"

void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	// Defensive: avoid dereferencing a null ASC. Caller normally ensures this, but
	// the crash stack showed an access violation here so we make this robust.
	if (!InASCToGive)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent called with null AbilitySystemComponent"));
		return;
	}

	// 1. Double-Initialization Guard
	if (InASCToGive->bStartupAbilitiesGiven)
	{
		return;
	}

	// 2. APPLY STATS FIRST (GEs) 
	// This ensures attributes like MaxHealth are set before abilities look for them
	for (const TSubclassOf<UGameplayEffect>& EffectClass : StartUpGameplayEffects)
	{
		if (!EffectClass) continue;

        FGameplayEffectContextHandle ContextHandle = InASCToGive->MakeEffectContext();

		// Only add a source object to the context if the avatar actor is valid. Some
		// ASC setups delay avatar assignment and AddSourceObject can crash if given
		// an invalid pointer.
		if (AActor* AvatarActor = InASCToGive->GetAvatarActor())
		{
			ContextHandle.AddSourceObject(AvatarActor);
		}

		FGameplayEffectSpecHandle SpecHandle = InASCToGive->MakeOutgoingSpec(EffectClass, (float)ApplyLevel, ContextHandle);

		if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
		{
			InASCToGive->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	// 3. GRANT ABILITIES SECOND
	GrantAbilities(ActivateOnGivenAbilities, InASCToGive, ApplyLevel);
	GrantAbilities(ReactiveAbilities, InASCToGive, ApplyLevel);

	// 4. Mark as finished
	InASCToGive->bStartupAbilitiesGiven = true;
}

void UDataAsset_StartUpDataBase::GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>>& AbilitiesToGive, UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	if (AbilitiesToGive.IsEmpty()) return;

	for (const TSubclassOf<UWarriorGameplayAbility>& AbilityClass : AbilitiesToGive)
	{
		if (!AbilityClass) continue;

		// UE 5.6 Fixed Constructor: Pass the Default Object and the Level explicitly
		FGameplayAbilitySpec AbilitySpec(AbilityClass.GetDefaultObject(), ApplyLevel);

		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();

		InASCToGive->GiveAbility(AbilitySpec);
	}
}