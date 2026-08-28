// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_OptionsScreen.generated.h"

class UOptionsDataRegistry;
class UFrontEndTabListWidgetBase;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class ADVANCEDRPG_API UWidget_OptionsScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;

private:

	UOptionsDataRegistry* GetOrCreateDataRegistry();

	void OnResetBoundActionTriggered();
	void OnBackBoundActionTriggered();
	
 
	/**  Bound Widgets  **/

	UPROPERTY(meta = (BindWidget))
	UFrontEndTabListWidgetBase* TabListWidget_OptionsTab;

	/**   Bound Widgets  **/


	// Handles the creation of data in the options screen and registers it with the data registry.Note direct access to this variable is forbidden
	UPROPERTY(Transient)
	UOptionsDataRegistry* OwningDataRegistry;



	UPROPERTY(EditDefaultsOnly, Category = "Front End Options Screen", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAction;

	FUIActionBindingHandle ResetActionHandle;
	
};
