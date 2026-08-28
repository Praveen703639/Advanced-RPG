// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "WarriorTypes/WarriorStructTypes.h"

#include "DataAsset_BowStartUpData.generated.h"

class UWarriorGameplayAbility;
class UWarriorAbilitySystemComponent;
class ACharacter;
class UBowCombatComponent;

/**
 * Bow-specific startup data asset. Keeps bow configuration separate from hero semantics.
 */
UCLASS()
class ADVANCEDRPG_API UDataAsset_BowStartUpData : public UDataAsset_StartUpDataBase
{
    GENERATED_BODY()

public:
    // Grants all startup abilities from this data asset to the specified ability system component.
    virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1) override;

    // Spawns and registers bow weapons (arrows) into the BowCombatComponent, mirroring the hero weapon startup.
    void GiveBowWeaponsToCombatComponent(ACharacter* InOwnerCharacter, UBowCombatComponent* InCombatComponent, int32 ApplyLevel = 1);

    /** Returns whether this startup data has any weapons configured. */
    UFUNCTION(BlueprintCallable, Category = "StartUpData")
    bool HasStartUpWeapons() const { return BowStartUpWeapons.Num() > 0; }

    /** Returns the number of startup weapons. */
    UFUNCTION(BlueprintCallable, Category = "StartUpData")
    int32 GetStartUpWeaponCount() const { return BowStartUpWeapons.Num(); }

private:
    // Array of abilities to grant when this bow character is initialized.
    UPROPERTY(EditDefaultsOnly, Category = "StartUpData", meta = (TitleProperty = "InputTag"))
    TArray<FWarriorHeroAbilitySet> BowStartUpAbilitySets;

    // Array of weapons to spawn/register when this bow character is initialized (optional reuse of existing struct)
    UPROPERTY(EditDefaultsOnly, Category = "StartUpData", meta = (TitleProperty = "WeaponTag"))
    TArray<FWarriorHeroStartUpWeapon> BowStartUpWeapons;
};
