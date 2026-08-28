// pravin's patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/WarriorWeaponBase.h"
#include "GameplayEffectTypes.h"
#include "WarriorArrow.generated.h"

class UProjectileMovementComponent;

/**
 * Arrow weapon for bow characters.
 * 
 * Key Features:
 * - Projectile movement component for flight physics
 * - Carries FGameplayEffectSpecHandle for damage application on hit
 * - Supports on-hit callbacks to apply damage/effects to target
 * 
 * Usage:
 * 1. Spawned by bow ability (GA_ShootBow)
 * 2. Flies via projectile movement
 * 3. On collision with target, applies stored effect spec handle
 * 4. Can be pooled/destroyed after impact
 */
UCLASS()
class ADVANCEDRPG_API AWarriorArrow : public AWarriorWeaponBase
{
    GENERATED_BODY()

public:
    AWarriorArrow();

    // Set the damage effect spec to apply on hit
    UFUNCTION(BlueprintCallable, Category = "Arrow")
    void SetDamageEffectSpecHandle(const FGameplayEffectSpecHandle& InDamageEffectSpecHandle);

    // Get the stored damage effect spec handle
    UFUNCTION(BlueprintCallable, Category = "Arrow")
    FGameplayEffectSpecHandle GetDamageEffectSpecHandle() const { return DamageEffectSpecHandle; }

    // Launch the arrow with initial velocity
    UFUNCTION(BlueprintCallable, Category = "Arrow")
    void LaunchArrow(const FVector& LaunchDirection, float LaunchSpeed);

protected:
    // Projectile movement component for realistic arrow flight
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    UProjectileMovementComponent* ProjectileMovement;

    // Damage effect spec to apply when arrow hits a target
    // Exposed to Blueprint so designers can see/edit it in editor
    UPROPERTY(BlueprintReadWrite, Category = "Damage", meta = (ExposeOnSpawn = true))
    FGameplayEffectSpecHandle DamageEffectSpecHandle;

    // Called when arrow collides with target
    virtual void OnCollisionBoxBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

    // Applies the stored damage effect to the target actor
    UFUNCTION(BlueprintCallable, Category = "Arrow")
    void ApplyDamageToTarget(AActor* HitActor);
};
