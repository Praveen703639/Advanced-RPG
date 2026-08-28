// pravin's patashala all Rights Reserved 

#include "Items/Weapons/WarriorArrow.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AWarriorArrow::AWarriorArrow()
{
    PrimaryActorTick.bCanEverTick = false;

    // Disable default collision from base weapon
    GetWeaponCollisionCapsule()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    GetWeaponCollisionCapsule()->SetCollisionResponseToAllChannels(ECR_Ignore);
    GetWeaponCollisionCapsule()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // Add projectile movement component for flight physics
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 0.f; // Set via LaunchArrow
    ProjectileMovement->MaxSpeed = 5000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
}

void AWarriorArrow::SetDamageEffectSpecHandle(const FGameplayEffectSpecHandle& InDamageEffectSpecHandle)
{
    DamageEffectSpecHandle = InDamageEffectSpecHandle;
}

void AWarriorArrow::LaunchArrow(const FVector& LaunchDirection, float LaunchSpeed)
{
    if (ProjectileMovement)
    {
        ProjectileMovement->Velocity = LaunchDirection.GetSafeNormal() * LaunchSpeed;
    }
}

void AWarriorArrow::OnCollisionBoxBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Get the instigator (the character who shot the arrow)
    APawn* ArrowOwner = GetInstigator<APawn>();

    // Don't hit the owner
    if (OtherActor == ArrowOwner) return;

    if (OtherActor != ArrowOwner && ProjectileMovement)
    {
        ProjectileMovement->StopMovementImmediately();
    }

    // Skip non-pawns (environment, etc.) unless you want arrows to stick/bounce
    if (!OtherActor->IsA<APawn>()) return;

    // Apply damage and call delegate
    ApplyDamageToTarget(OtherActor);
    OnWeaponHitTarget.ExecuteIfBound(OtherActor);

    // Optionally stop the arrow or destroy it
    if (ProjectileMovement)
    {
        ProjectileMovement->StopMovementImmediately();
    }

    // Destroy the arrow after impact (or use pooling)
    Destroy();
}

void AWarriorArrow::ApplyDamageToTarget(AActor* HitActor)
{
    if (!HitActor || !DamageEffectSpecHandle.IsValid())
    {
        return;
    }

    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
    if (TargetASC)
    {
        // Apply the damage effect to the target
        TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
        
        UE_LOG(LogTemp, Warning, TEXT("Arrow applied damage to: %s"), *HitActor->GetName());
    }
}
