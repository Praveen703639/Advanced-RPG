// pravin's patashala all Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameplayTagContainer.h"
#include "AbilityTask_TransformCharacter.generated.h"

// Forward declarations
class AWarriorHeroCharacter;
class AWarriorBowCharacter;
class AWarriorWeaponBase;
class UParticleSystemComponent;
class ACameraActor;
class UAnimMontage;
class UParticleSystem;

UENUM(BlueprintType)
enum class ETransformType : uint8
{
    HeroToBow   UMETA(DisplayName = "Hero to Bow"),
    BowToHero   UMETA(DisplayName = "Bow to Hero")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTransformFailed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTransformSucceeded, AActor*, NewCharacter);

/**
 * Ability Task that handles smooth character transformation between Hero and Bow forms.
 * Includes cinematic camera, dissolve/form effects, montage playback, and weapon transfer.
 */
UCLASS()
class ADVANCEDRPG_API UAbilityTask_TransformCharacter : public UAbilityTask
{
    GENERATED_BODY()

public:
    /**
     * Creates a transform task that swaps the player between Hero and Bow characters.
     */
    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks|Transform", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
    static UAbilityTask_TransformCharacter* TransformCharacter(
        UGameplayAbility* OwningAbility,
        ETransformType TransformType,
        TSoftClassPtr<AWarriorHeroCharacter> HeroClass,
        TSoftClassPtr<AWarriorBowCharacter> BowClass,
        UAnimMontage* SourceOutMontage,
        UAnimMontage* TargetInMontage,
        UParticleSystem* DissolveEffect,
        UParticleSystem* FormEffect,
        float TransformDuration = 1.0f,
        bool bUseCinematicCamera = false,
        TSubclassOf<ACameraActor> CinematicCameraClass = nullptr);

    /** Called when the transformation fails at any point */
    UPROPERTY(BlueprintAssignable)
    FOnTransformFailed OnTransformFailed;

    /** Called when the transformation completes successfully */
    UPROPERTY(BlueprintAssignable)
    FOnTransformSucceeded OnTransformSucceeded;

    /** Sets the weapon to transfer during HeroToBow transformation */
    UFUNCTION(BlueprintCallable, Category = "Transform")
    void SetWeaponToTransfer(AWarriorWeaponBase* Weapon, FGameplayTag WeaponTag);

    /** Sets camera shake parameters (reserved for future implementation) */
    UFUNCTION(BlueprintCallable, Category = "Transform")
    void SetCameraShakeParams(float Intensity, float Duration, float BlendIn, float BlendOut);

protected:
    //~ Begin UAbilityTask Interface
    virtual void Activate() override;
    virtual void OnDestroy(bool bInOwnerFinished) override;
    //~ End UAbilityTask Interface

private:
    // ==================== CONFIGURATION ====================

    UPROPERTY()
    ETransformType CachedTransformType;

    UPROPERTY()
    TSoftClassPtr<AWarriorHeroCharacter> CachedHeroClass;

    UPROPERTY()
    TSoftClassPtr<AWarriorBowCharacter> CachedBowClass;

    UPROPERTY()
    TObjectPtr<UAnimMontage> CachedSourceOutMontage;

    UPROPERTY()
    TObjectPtr<UAnimMontage> CachedTargetInMontage;

    UPROPERTY()
    TObjectPtr<UParticleSystem> CachedDissolveEffect;

    UPROPERTY()
    TObjectPtr<UParticleSystem> CachedFormEffect;

    UPROPERTY()
    float CachedTransformDuration;

    UPROPERTY()
    bool bCachedUseCinematicCamera;

    UPROPERTY()
    TSubclassOf<ACameraActor> CachedCinematicCameraClass;

    // ==================== WEAPON TRANSFER ====================

    /** Weapon to transfer to the bow character */
    UPROPERTY()
    TObjectPtr<AWarriorWeaponBase> WeaponToTransfer;

    /** Gameplay tag identifying the weapon slot */
    UPROPERTY()
    FGameplayTag TransferWeaponTag;

    // ==================== CAMERA SETTINGS ====================

    /** Offset for cinematic camera relative to character */
    UPROPERTY(EditDefaultsOnly, Category = "Transform|Camera")
    FVector CachedCameraOffset = FVector(-200.0f, 0.0f, 150.0f);

    /** Time to blend into cinematic camera */
    UPROPERTY(EditDefaultsOnly, Category = "Transform|Camera")
    float CameraBlendInTime = 0.5f;

    /** Time to blend out from cinematic camera */
    UPROPERTY(EditDefaultsOnly, Category = "Transform|Camera")
    float CameraBlendOutTime = 0.5f;

    // ==================== TIMING SETTINGS ====================

    /** Ratio of out-montage duration at which to spawn new character (0.0 - 1.0) */
    UPROPERTY(EditDefaultsOnly, Category = "Transform|Timing")
    float MontageToSpawnRatio = 0.8f;

    /** Minimum delay before spawning new character */
    UPROPERTY(EditDefaultsOnly, Category = "Transform|Timing")
    float MinSpawnDelay = 0.1f;

    /** Delay after possession before playing in-montage */
    UPROPERTY(EditDefaultsOnly, Category = "Transform|Timing")
    float PostPossessDelay = 0.1f;

    /** Minimum delay before restoring player control */
    UPROPERTY(EditDefaultsOnly, Category = "Transform|Timing")
    float MinRestoreControlDelay = 0.5f;

    // ==================== RUNTIME STATE ====================

    UPROPERTY()
    TObjectPtr<AActor> SourceCharacter;

    UPROPERTY()
    TObjectPtr<AActor> SpawnedCharacter;

    UPROPERTY()
    TObjectPtr<APlayerController> OwningController;

    UPROPERTY()
    TObjectPtr<ACameraActor> CinematicCamera;

    UPROPERTY()
    TObjectPtr<UParticleSystemComponent> SpawnedFormEffectComponent;

    /** Cached location at start of transformation */
    UPROPERTY()
    FVector CachedTransformLocation;

    /** Cached rotation at start of transformation */
    UPROPERTY()
    FRotator CachedTransformRotation;

    /** Full cached transform at start */
    UPROPERTY()
    FTransform CachedSpawnTransform;

    /** Whether transformation is currently in progress */
    bool bIsTransforming = false;

    /** Whether transformation has failed */
    bool bTransformFailed = false;

    /** Whether new character has already been spawned (prevents double-spawn) */
    bool bHasSpawnedNewCharacter = false;

    /** Whether control restoration is in progress (prevents double-execution) */
    bool bIsRestoringControl = false;

    /** Timer handle for spawning new character */
    FTimerHandle SpawnTimer;

    /** Timer handle for form/in-montage sequence */
    FTimerHandle FormTimer;

    // ==================== SEQUENCE METHODS ====================

    void StartTransformSequence();
    void PlayOutMontageAndDissolve();
    void OnOutMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    void SpawnNewCharacter();
    void PlayInMontageAndForm();
    void OnInMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    void RestorePlayerControl();
    void CleanupCinematicCamera();
};