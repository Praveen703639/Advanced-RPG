// pravin's patashala all Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/PawnCombatComponent.h"

#include "BowCombatComponent.generated.h"

class AWarriorArrow;
class AWarriorWeaponBase;

UCLASS()
class ADVANCEDRPG_API UBowCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat|Bow")
	AWarriorArrow* GetBowCarriedArrowByTag(FGameplayTag InArrowTagToGet) const;

	// NO UFUNCTION — parent already has it
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon = false);

	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat|Bow")
	AWarriorArrow* GetBowCurrentEquippedArrow() const;

	// Virtuals declared here, defined in .cpp
	virtual void OnHitTargetActor(AActor* HitActor) override;
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bow|Visuals")
	TObjectPtr<class UNiagaraSystem> ArrowHitSparkEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bow|Audio")
	TObjectPtr<class USoundBase> ArrowHitSoundEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bow|Audio")
	TObjectPtr<class USoundBase> BowDrawSoundEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bow|Audio")
	TObjectPtr<class USoundBase> BowFireSoundEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bow|Visuals ")
	FName WeaponAttachSocketName = FName("Hand_R");

protected:
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat|Bow")
	FName GetWeaponAttachSocketName(FGameplayTag WeaponTag) const;
};