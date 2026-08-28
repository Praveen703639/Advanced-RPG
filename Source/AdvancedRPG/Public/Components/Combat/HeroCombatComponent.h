// Pravin's Pathshala - All Rights Reserved
#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "GameplayTagContainer.h"
#include "HeroCombatComponent.generated.h"

class AWarriorHeroWeapon;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ADVANCEDRPG_API UHeroCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()

public:
	UHeroCombatComponent();

	UFUNCTION(BlueprintCallable)
	AWarriorHeroWeapon* GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTagToGive) const;

	UFUNCTION(BlueprintCallable)
	AWarriorHeroWeapon* GetHeroCurrentEquippedWeapon() const;

	UFUNCTION(BlueprintCallable)
	float GetHeroCurrentEquippedWeaponDamageAtLevel(float InLevel) const;

	virtual void OnHitTargetActor(AActor* HitActor) override;
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor) override;

	UFUNCTION(BlueprintCallable)
	void ResetHitActors();

protected:
	// CRITICAL: Rebinds weapon delegates to HERO hit functions
	virtual void PostRegisterWeapon(AWarriorWeaponBase* InWeaponToRegister) override;
};