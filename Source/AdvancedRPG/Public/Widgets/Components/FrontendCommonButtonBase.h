// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "FrontendCommonButtonBase.generated.h"
class UCommonTextBlock;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class ADVANCEDRPG_API UFrontendCommonButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetButtonText(FText InText);


private:
	virtual void NativePreConstruct() override;

	virtual void NativeOnCurrentTextStyleChanged()override;
	virtual void NativeOnHovered()override;
	virtual void NativeOnUnhovered()override;

	//** Bound Widgets **//
	UPROPERTY(meta = (BindWidgetOptional))
	UCommonTextBlock* CommonTextBlock_ButtonText;
	//** Bound Widgets **//

	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend  Button", meta = (AllowPrivateAccess = "true"))
	FText ButtonDisplayText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend  Button", meta = (AllowPrivateAccess = "true"))
	bool bUserUpperCaseForButtonText = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend  Button", meta = (AllowPrivateAccess = "true"))
	FText ButtonDescriptionText;



};
