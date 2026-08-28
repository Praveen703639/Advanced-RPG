// pravin's patashala all Rights Reserved 

#include "AbilitySystem/WarriorAttributeSet.h"
#include "GameplayEffectExtension.h" // Required for FGameplayEffectModCallbackData
#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "warriorGameplayTags.h"
#include "Interfaces/PawnUIInterface.h"
#include "Components/UI/PawnUIComponents.h"
#include "Components/UI/HeroUIComponent.h"

UWarriorAttributeSet::UWarriorAttributeSet()
{
	InitCurrentHealth(1.f);
	InitMaxHealth(1.f);
	InitCurrentRage(1.f);
	InitMaxRage(1.f);
	InitAttackPower(1.f);
	InitDefencePower(1.f);
}

void UWarriorAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (!CachedPawnUIInterface.IsValid())
	{
		CachedPawnUIInterface = TWeakInterfacePtr<IPawnUIInterface>(Data.Target.GetAvatarActor());
	}

    checkf(CachedPawnUIInterface.IsValid(), TEXT("%s Didnot implement the IPawnUIInterface"), *Data.Target.GetAvatarActor()->GetActorNameOrLabel());
	UPawnUIComponents* PawnUIComponent = CachedPawnUIInterface->GetPawnUIComponents();

	checkf(PawnUIComponent, TEXT("Could not extract a PawnUIComponent from %s: "), *Data.Target.GetAvatarActor()->GetActorNameOrLabel());


	Super::PostGameplayEffectExecute(Data);

	// 1. Handle Health Changes
	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		const float NewCurrentHealth = FMath::Clamp(GetCurrentHealth(), 0.f, GetMaxHealth());
		SetCurrentHealth(NewCurrentHealth);

		// Compute percent safely (avoid divide by zero) and log values for debugging UI issues
		const float HealthPercent = (GetMaxHealth() > 0.f) ? (GetCurrentHealth() / GetMaxHealth()) : 0.f;
		{
			const FString DebugString = FString::Printf(TEXT("Broadcasting Health Percent: Current=%f Max=%f Percent=%f"),
				GetCurrentHealth(), GetMaxHealth(), HealthPercent);
			//Debug::Print(DebugString, FColor::Cyan);
		}

		PawnUIComponent->OnCurrentHealthChnaged.Broadcast(HealthPercent);	


	}

	

	// 2. Handle Rage Changes
	if (Data.EvaluatedData.Attribute == GetCurrentRageAttribute())
	{
		const float NewCurrentRage = FMath::Clamp(GetCurrentRage(), 0.f, GetMaxRage());
		SetCurrentRage(NewCurrentRage);

		if (GetCurrentRage() == GetMaxRage())
		{
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), WarriorGameplayTags::Player_Status_Rage_Full);
		}

		else if(GetCurrentRage() == 0.f)
		{
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), WarriorGameplayTags::Player_Status_Rage_None);
		}
		else
		{
			UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(Data.Target.GetAvatarActor(), WarriorGameplayTags::Player_Status_Rage_Full);
			UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(Data.Target.GetAvatarActor(), WarriorGameplayTags::Player_Status_Rage_None);
		}



		if (UHeroUIComponent* HeroUIComponent = CachedPawnUIInterface->GetHeroUIComponent())
		{
			const float RagePercent = (GetMaxRage() > 0.f) ? (GetCurrentRage() / GetMaxRage()) : 0.f;
			const FString DebugString = FString::Printf(TEXT("Broadcasting Rage Percent: Current=%f Max=%f Percent=%f"),
				GetCurrentRage(), GetMaxRage(), RagePercent);
			//Debug::Print(DebugString, FColor::Cyan);
			HeroUIComponent->OnCurrentRageChanged.Broadcast(RagePercent);
		}


	}


	// 3. Handle DamageTaken  Changes

	if (Data.EvaluatedData.Attribute ==GetDamageTakenAttribute())
	{
		const float OldHealth = GetCurrentHealth();
		const float DamageDone = GetDamageTaken();

		const float NewCurrentHealth = FMath::Clamp(OldHealth - DamageDone, 0.f, GetMaxHealth());
		SetCurrentHealth(NewCurrentHealth);

		

	

		const FString DebugString = FString::Printf(
			TEXT("OldHealth is :%f, DamageDone is :%f, NewCurrentHealth is:%f"),
				OldHealth,
				DamageDone,
				NewCurrentHealth
		);

		//Debug::Print(DebugString, FColor::Green);

		PawnUIComponent->OnCurrentHealthChnaged.Broadcast(GetCurrentHealth() / GetMaxHealth());


		if (GetCurrentHealth() == 0.f)
		{
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), WarriorGameplayTags::Shared_Status_Death);
			
		}

	}

	




}