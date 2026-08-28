// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WarriorWidgetBase.generated.h"

class UHeroUIComponent;
class UEnemyUIComponent;
/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UWarriorWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = " On Owning Hero UI Component Initialized "))
	void BP_OnOwningHeroUIComponentInitialized(UHeroUIComponent* OwningHeroUIComponent);

public:
	// Blueprint callable helper to debug incoming percent values (0..1). Bind in widget blueprint to verify received values.
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void Debug_OnHealthPercent(float Percent);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = " On Owning Enemy UI Component Initialized "))
	void BP_OnOwningEnemyUIComponentInitialized(UEnemyUIComponent* OwningEnemyUIComponent);


public:

	UFUNCTION(BlueprintCallable)
	void InitEnemyCreatedWidget(AActor* OwningEnemyActor);



};
