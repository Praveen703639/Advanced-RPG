// pravin's patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarriorWeaponBase.generated.h"

class UStaticMeshComponent;
class UCapsuleComponent;


DECLARE_DELEGATE_OneParam(FOnTargetInteractedDelegate,AActor* /*HitActor*/)



UCLASS()
class ADVANCEDRPG_API AWarriorWeaponBase : public AActor
{
    GENERATED_BODY()

public:
    AWarriorWeaponBase();

    FOnTargetInteractedDelegate OnWeaponHitTarget;
	FOnTargetInteractedDelegate OnWeaponPulledFromTarget;


protected:

    // Make the mesh editable in Blueprints and instances so designers can adjust rotation/transform
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta=(AllowPrivateAccess="true"))
    UStaticMeshComponent* WeaponMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    UCapsuleComponent* WeaponCollisionCapsule;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    USceneComponent* Root;

    UFUNCTION()
    virtual void OnCollisionBoxBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    
    UFUNCTION()
    
    virtual void OnCollisionBoxEndOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:
    FORCEINLINE UCapsuleComponent* GetWeaponCollisionCapsule() const { return WeaponCollisionCapsule; }
};