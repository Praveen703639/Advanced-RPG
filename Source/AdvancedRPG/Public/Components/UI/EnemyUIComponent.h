// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/PawnUIComponents.h"
#include "EnemyUIComponent.generated.h"

class UWarriorWidgetBase;

/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UEnemyUIComponent : public UPawnUIComponents
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void RegisterEnemyDrawnWidget(UWarriorWidgetBase* InWidgetToRegister);

	UFUNCTION(BlueprintCallable)
	void RemoveEnemyDrawnWidgetsIfAny();

private:
	TArray<UWarriorWidgetBase*> EnemyDrawnWidgets;
	
};
