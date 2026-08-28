// Pravin's Pathshala - All Rights Reserved
#include "Components/Combat/HeroCombatComponent.h"
#include "Items/Weapons/WarriorHeroWeapon.h"
#include "Items/Weapons/WarriorWeaponBase.h"
#include "warriorGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "WarriorFunctionLibrary.h"

#include "WarriorDebugHelper.h"

UHeroCombatComponent::UHeroCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTagToGive) const
{
	return Cast<AWarriorHeroWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTagToGive));
}

AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCurrentEquippedWeapon() const
{
	return Cast<AWarriorHeroWeapon>(GetCharacterCurrentEquippedWeapon());
}

float UHeroCombatComponent::GetHeroCurrentEquippedWeaponDamageAtLevel(float InLevel) const
{
	if (AWarriorHeroWeapon* CurrentWeapon = GetHeroCurrentEquippedWeapon())
	{
		return UWarriorFunctionLibrary::GetScalableFloatValueAtLevel(
			CurrentWeapon->HeroWeaponData.WeaponBaseDamage,
			InLevel
		);
	}
	return 0.f;
}

// CRITICAL FIX: Rebinds delegates to HERO functions so your hit effects & GAS events actually fire
void UHeroCombatComponent::PostRegisterWeapon(AWarriorWeaponBase* InWeaponToRegister)
{
	if (InWeaponToRegister)
	{
		InWeaponToRegister->OnWeaponHitTarget.Unbind();
		InWeaponToRegister->OnWeaponHitTarget.BindUObject(this, &UHeroCombatComponent::OnHitTargetActor);

		InWeaponToRegister->OnWeaponPulledFromTarget.Unbind();
		InWeaponToRegister->OnWeaponPulledFromTarget.BindUObject(this, &UHeroCombatComponent::OnWeaponPulledFromTargetActor);
	}
}

void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	// =========================
	// SAFETY CHECK
	// =========================
	if (!HitActor || HitActor == GetOwningPawn())
	{
		return;
	}

	// Prevent multi-hit on the same actor per swing
	// OverlappedActors is cleared when weapon collision is enabled (start of each attack)
	if (OverlappedActors.Contains(HitActor))
	{
		return;
	}

	// Register this actor as hit for this swing (prevents duplicate events)
	OverlappedActors.AddUnique(HitActor);

	const FVector HitLocation = HitActor->GetActorLocation();

	// =========================
	// PLAY EFFECTS
	// =========================

	if (AWarriorHeroWeapon* CurrentWeapon = GetHeroCurrentEquippedWeapon())
	{
		// --- NIAGARA HIT SPARK ---
		if (UNiagaraSystem* HitEffect = CurrentWeapon->HeroWeaponData.HitSparkEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				HitEffect,
				HitLocation,
				GetOwningPawn()->GetActorRotation()
			);
		}

		// --- HIT SOUND ---
		if (USoundBase* HitSound = CurrentWeapon->HeroWeaponData.HitSoundEffect)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				HitSound,
				HitLocation
			);
		}
	}

	// =========================
	// SEND GAS MELEE HIT EVENT
	// Sent to the HERO (owning pawn) so the hero's active attack ability
	// can respond (apply damage GE to the HitActor, trigger hit-pause, etc.)
	// Data.Target is the enemy that was hit.
	// =========================
	FGameplayEventData Data;
	Data.Instigator = GetOwningPawn();
	Data.Target = HitActor;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		WarriorGameplayTags::Shared_Event_MeeleHit,
		Data
	);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		WarriorGameplayTags::Player_Event_HitPause,
		FGameplayEventData()
	);
}

void UHeroCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
	// Nothing special needed for hero when weapon leaves an actor's volume.
	// The swing-end is handled by disabling collision (TogggleWeaponCollision false),
	// which already clears OverlappedActors in the base class.
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		WarriorGameplayTags::Player_Event_HitPause,
		FGameplayEventData()
	);
}

void UHeroCombatComponent::ResetHitActors()
{
	OverlappedActors.Empty();
}