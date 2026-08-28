// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEExecCalc_DamageTaken.generated.h"


/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UGEExecCalc_DamageTaken : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UGEExecCalc_DamageTaken();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
