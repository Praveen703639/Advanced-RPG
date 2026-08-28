// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "WarriorProjectileTypes.generated.h"

UENUM(BlueprintType)
enum class EWarriorProjectileDamagePolicy : uint8
{
    OnHit,
    OnBeginOverlap
};
