// pravin's patashala all Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Misc/Optional.h"
#include "GameplayEffectTypes.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "WarriorGameplayAbility.generated.h"

class UPawnCombatComponent;
class UWarriorAbilitySystemComponent;
class AWarriorHeroCharacter;
class AWarriorBowCharacter;
class ACameraActor;
class UAnimMontage;
class UParticleSystem;
class AWarriorEnemyCharacter;

UENUM(BlueprintType)
enum class EWarriorAbilityActivationPolicy : uint8
{
    OnTriggered,
    OnGiven,
};

UENUM(BlueprintType)
enum class EWarriorTransformType : uint8
{
    None          UMETA(DisplayName = "None"),
    HeroToBow     UMETA(DisplayName = "Hero -> Bow"),
    BowToHero     UMETA(DisplayName = "Bow -> Hero")
};

UCLASS()
class ADVANCEDRPG_API UWarriorGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UWarriorGameplayAbility();

    // === TRANSFORM SYSTEM ===
    // Blueprint-callable transform execution
    UFUNCTION(BlueprintCallable, Category = "Warrior|Transform")
    void ExecuteTransform(EWarriorTransformType TransformType);

    // Get current transform direction based on avatar actor class
    UFUNCTION(BlueprintPure, Category = "Warrior|Transform")
    EWarriorTransformType GetAutoTransformType() const;

protected:
    //~ Begin UGameplayAbility Interface.
    virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
    //~ End UGameplayAbility Interface

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Warrior|Ability")
    EWarriorAbilityActivationPolicy ActivationPolicy = EWarriorAbilityActivationPolicy::OnTriggered;

    // === TRANSFORM CONFIG (set in child BP defaults) ===
    UPROPERTY(EditDefaultsOnly, Category = "Warrior|Transform")
    TSoftClassPtr<AWarriorHeroCharacter> HeroClass;

    UPROPERTY(EditDefaultsOnly, Category = "Warrior|Transform")
    TSoftClassPtr<AWarriorBowCharacter> BowClass;

    UPROPERTY(EditDefaultsOnly, Category = "Warrior|Transform|Effects")
    UParticleSystem* DissolveEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Warrior|Transform|Effects")
    UParticleSystem* FormEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Warrior|Transform|Camera")
    TSubclassOf<ACameraActor> CinematicCameraClass;

    UPROPERTY(EditDefaultsOnly, Category = "Warrior|Transform|Camera")
    float TransformDuration = 2.5f;

    // === TRANSFORM MONTAGES (override in child classes) ===
    UPROPERTY(EditDefaultsOnly, Category = "Warrior|Transform|Montages")
    UAnimMontage* HeroToBow_OutMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Warrior|Transform|Montages")
    UAnimMontage* HeroToBow_InMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Warrior|Transform|Montages")
    UAnimMontage* BowToHero_OutMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Warrior|Transform|Montages")
    UAnimMontage* BowToHero_InMontage;

    // Virtual hook for post-transform logic (ability grants, etc.)
    UFUNCTION(BlueprintImplementableEvent, Category = "Warrior|Transform")
    void OnTransformComplete(AActor* NewCharacter);

    UFUNCTION(BlueprintImplementableEvent, Category = "Warrior|Transform")
    void OnTransformFailed();

    UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
    UPawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;

    UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
    UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponentFromActorInfo() const;

    FActiveGameplayEffectHandle NativeApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle);

    UFUNCTION(BlueprintCallable, Category = "Warrior|Ability", meta = (DisplayName = "Apply Gameplay Effect SpecHandle To Target Actor", ExpandEnumAsExecs = "OutSuccessType"))
    FActiveGameplayEffectHandle BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle, EWarriorSuccessType& OutSuccessType);

    UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
    void ApplyGameplayEfffectSpecHandleToHitResult(const FGameplayEffectSpecHandle& InSpecHandle, const TArray<FHitResult>& InHitResults);

    

/**
 * Takes hit results from a multi-box trace, filters for hostile WarriorEnemyCharacters,
 * and returns them. Also optionally spawns a Cascade ice VFX on each enemy.
 * Similar to ApplyGameplayEfffectSpecHandleToHitResult but returns the enemies instead.
 */
 /**
 * 
* Takes hit results from multi-box trace, filters hostile WarriorEnemyCharacters,
* pauses their animation, zeroes velocity, spawns ice VFX, and returns them.
*/
    UFUNCTION(BlueprintCallable, Category = "Warrior|Ability|Frost")
    TArray<AWarriorEnemyCharacter*> GetEnemyCharactersFromHitResults(
        const FGameplayEffectSpecHandle& InSpecHandle,      // Kept for consistency
        const TArray<FHitResult>& InHitResults,
        UParticleSystem* InIceVFX,
        FName InAttachSocketName = FName("root"));

    /** Cascade particle effect to spawn on frozen enemies. Set in Blueprint defaults. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Warrior|Ability|Frost")
    TObjectPtr<UParticleSystem> IceFreezeVFX;

    /** Duration enemies stay frozen (animation paused). 0 = permanent until manually unfrozen. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Warrior|Ability|Frost")
    float FreezeDuration = 8.0f;

    /** Timer handle to auto-unfreeze enemies after FreezeDuration. */
    UPROPERTY()
    FTimerHandle UnfreezeTimerHandle;

    /**
 * Takes hit results from multi-box trace, filters hostile WarriorEnemyCharacters,
 * freezes them (stops AI, zeroes velocity, pauses anim), spawns ice VFX, and returns them.
 */
   

 // Unfreeze all tracked enemies
    UFUNCTION()
    void UnfreezeAllTrackedEnemies();

    // Track frozen enemies so we can unfreeze them later
    UPROPERTY()
    TArray<AWarriorEnemyCharacter*> FrozenEnemies;

    UFUNCTION()
    void AttachIceVFXToEnemy(AWarriorEnemyCharacter* InEnemy, UParticleSystem* InIceVFX, FName InAttachSocketName);

   
};