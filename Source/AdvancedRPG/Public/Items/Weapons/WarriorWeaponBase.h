// pravin's patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarriorWeaponBase.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class ADVANCEDRPG_API AWarriorWeaponBase : public AActor
{
    GENERATED_BODY()

public:
    AWarriorWeaponBase();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    UStaticMeshComponent* WeaponMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    UBoxComponent* WeaponCollisionBox; 

public:
    FORCEINLINE UBoxComponent* GetWeaponCollisionBox() const { return WeaponCollisionBox; }
};