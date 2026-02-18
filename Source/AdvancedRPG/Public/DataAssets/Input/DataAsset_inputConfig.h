// pravin's patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DataAsset_inputConfig.generated.h"

class UInputAction;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct FWarriorInputActionConfig
{
    GENERATED_BODY()

public:
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Category = "Input"))
    FGameplayTag InputTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UInputAction* InputAction;
};

/**
 * */
UCLASS()
class ADVANCEDRPG_API UDataAsset_inputConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UInputMappingContext* DefaultMappingContext;


    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
    TArray<FWarriorInputActionConfig> NativeInputActions;

   
    UInputAction* FindNativeInputActionByTag(const FGameplayTag& InInputTag) const;
};