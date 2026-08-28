// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/PawnUIComponents.h"
#include "GameplayTagContainer.h"
#include "WarriorTypes/WarriorStructTypes.h"


#include "HeroUIComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedWeaponChangedDelegate, TSoftObjectPtr<UTexture2D>, SoftWeaponIcon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpecialAbilityIconSlotUpdatedDelegate,  FGameplayTag,AbilityInputTag, TSoftObjectPtr<UMaterialInterface>, SoftSpecialAbilityIconMaterial);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAbilityCoolDownBeginDelegate, FGameplayTag, AbilityInputTag, float, TotalCoolDownTime, float, RemainingCoolDownTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoneInteractedDelegate, bool, bShouldDisplayInputKey);

/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UHeroUIComponent : public UPawnUIComponents
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FWarriorHeroAbilitySet HeroAbilitySet;

	UPROPERTY(BlueprintAssignable, Category = "UI")
	FOnPercentChangedDelegate OnCurrentRageChanged;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnEquippedWeaponChangedDelegate OnEquippedWeaponChanged;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnSpecialAbilityIconSlotUpdatedDelegate OnSpecialAbilityIconSlotUpdated;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnAbilityCoolDownBeginDelegate OnAbilityCoolDownBegin;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnStoneInteractedDelegate OnStoneInteracted;
};
