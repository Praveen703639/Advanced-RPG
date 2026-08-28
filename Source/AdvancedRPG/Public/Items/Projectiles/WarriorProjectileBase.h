// pravin's patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "WarriorProjectileTypes.h"

#include "WarriorProjectileBase.generated.h"

class UBoxComponent;
class UNiagaraComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;
struct FGameplayEventData;

UCLASS()
class ADVANCEDRPG_API AWarriorProjectileBase : public AActor
{
	GENERATED_BODY()

public:

	AWarriorProjectileBase();

	// Setters for AbilityTask / external configuration
	UFUNCTION(BlueprintCallable, Category = "Projectile|Setup")
	void SetProjectileVelocity(const FVector& InVelocity);

	UFUNCTION(BlueprintCallable, Category = "Projectile|Setup")
	void SetProjectileSpeed(float InInitialSpeed, float InMaxSpeed);

	UFUNCTION(BlueprintCallable, Category = "Projectile|Setup")
	void SetProjectileGravityScale(float InGravityScale);

	UFUNCTION(BlueprintCallable, Category = "Projectile|Setup")
	void SetDamageEffectSpecHandle(const FGameplayEffectSpecHandle& InSpecHandle);

	UFUNCTION(BlueprintCallable, Category = "Projectile|Setup")
	void SetNiagaraEffect(UNiagaraSystem* InNiagaraSystem);

	UFUNCTION(BlueprintCallable, Category = "Projectile|Setup")
	void ActivateNiagaraComponent(bool bReset = false);


	UFUNCTION(BlueprintCallable, Category = "Projectile|Setup")
	void SetHomingTarget(USceneComponent* InTarget, float InAccelerationMagnitude);

	UFUNCTION(BlueprintCallable, Category = "Projectile|Setup")
	void ClearHomingTarget();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	UBoxComponent* ProjectileCollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	UNiagaraComponent* ProjectileNiagaraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	UProjectileMovementComponent* ProjectileMovementComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	EWarriorProjectileDamagePolicy ProjectileDamagePolicy = EWarriorProjectileDamagePolicy::OnHit;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (ExposeOnSpawn = "true"))
	FGameplayEffectSpecHandle ProjectileDamageEffectSpecHandle;


	UFUNCTION()
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	UFUNCTION()
	virtual void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);


	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Spawn Projectile Hit FX"))
	void BP_OnSpawnProjectileHitFX(const FVector& HitLocation);

private:

	void HandleApplyProjectileDamage(APawn* InHitPawn, const FGameplayEventData& InPayload);
	TArray<AActor*> OverlappedActors;


};