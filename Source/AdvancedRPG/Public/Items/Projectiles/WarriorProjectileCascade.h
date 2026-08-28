// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "Items/Projectiles/WarriorProjectileTypes.h"
#include "WarriorProjectileCascade.generated.h"


class UBoxComponent;
class UParticleSystemComponent;
class UProjectileMovementComponent;
struct FGameplayEventData;

UCLASS()
class ADVANCEDRPG_API AWarriorProjectileCascade : public AActor
{
	GENERATED_BODY()
	

public:	
	
	AWarriorProjectileCascade();

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	UBoxComponent* ProjectileCollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	UParticleSystemComponent* ProjectileParticleSystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	UProjectileMovementComponent* ProjectileMovementComp;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
    EWarriorProjectileDamagePolicy ProjectileDamagePolicy = EWarriorProjectileDamagePolicy::OnHit;

	UPROPERTY(BlueprintReadOnly, Category = "Projectile", meta = (ExposeOnSpawn = "true"))
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
	TArray<AActor*> OverLappedActors;

};
