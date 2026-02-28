// pravin's patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnExtensionComponentBase.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ADVANCEDRPG_API UPawnExtensionComponentBase : public UActorComponent
{
    GENERATED_BODY()

protected:
    template <class T> 
    T* GetOwningPawn()
    {
        static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value, "T must be derived from APawn");
        return CastChecked<T>(GetOwner());
    }


    APawn* GetOwningPawn() const
    {
        return CastChecked<APawn>(GetOwner());
    }

    template <class T>
    T* GetOwningController()
    {
        static_assert(TPointerIsConvertibleFromTo<T, AController>::Value, "T must be derived from AController");

        
		return GetOwningPawn<T>()->GetController<T>();
    }
};