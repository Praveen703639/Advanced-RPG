// Pravin's Pathshala - All Rights Reserved
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "PawnCombatComponent.generated.h"

class AWarriorWeaponBase;

UENUM(BlueprintType)
enum class EToggleDamageType : uint8
{
	CurrentEquippedWeapon	UMETA(DisplayName = "Current Equipped Weapon"),
	LeftHand				UMETA(DisplayName = "Left Hand"),
	RightHand				UMETA(DisplayName = "Right Hand"),
	RightLeg				UMETA(DisplayName = "Right Leg"),
	LeftLeg					UMETA(DisplayName = "Left Leg")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ADVANCEDRPG_API UPawnCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPawnCombatComponent();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	APawn* GetOwningPawn() const { return Cast<APawn>(GetOwner()); }

	template<typename T>
	T* GetOwningPawn() const { return Cast<T>(GetOwner()); }

	// KEPT EXACTLY AS YOUR ORIGINAL — Blueprint node will NOT break
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon = false);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	const TMap<FGameplayTag, AWarriorWeaponBase*>& GetCharacterCarriedWeaponMap() const { return CharacterCarriedWeaponMap; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	FGameplayTag GetCurrentEquippedWeaponTag() const { return CurrentEquippedWeaponTag; }

	UFUNCTION(BlueprintCallable)
	AWarriorWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;

	UFUNCTION(BlueprintCallable)
	AWarriorWeaponBase* GetCharacterCurrentEquippedWeapon() const;

	UFUNCTION(BlueprintCallable)
	void TogggleWeaponCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType);

	virtual void OnHitTargetActor(AActor* HitActor);
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor);

	// BlueprintReadWrite because you set it in BP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FGameplayTag CurrentEquippedWeaponTag;

protected:
	// NEW: virtual hook so HeroCombatComponent can rebind delegates WITHOUT touching RegisterSpawnedWeapon
	virtual void PostRegisterWeapon(AWarriorWeaponBase* InWeaponToRegister);

	virtual void ToggleCurrentEquippedWeaponCollision(bool bShouldEnable);
	virtual void ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	TMap<FGameplayTag, AWarriorWeaponBase*> CharacterCarriedWeaponMap;

	UPROPERTY()
	TArray<AActor*> OverlappedActors;
};