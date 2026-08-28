// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "FrontEndTypes/FrontEndEnumTypes.h"
#include "Widget_ConfirmScreen.generated.h"
class UCommonTextBlock;
class UDynamicEntryBox;

USTRUCT(BlueprintType)
struct FConfirmScreenButtonInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EConfirmScreenButtonType ConfirmScreenButtonType = EConfirmScreenButtonType::Unknown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ButtonTextToDisplay;

};
UCLASS()
class ADVANCEDRPG_API UConfirmScreenInfoObject : public UObject
{
	GENERATED_BODY()
public:

	static UConfirmScreenInfoObject* CreateOkScreen(const FText& InScreenTitle, const FText& InScreenMessage);
	static UConfirmScreenInfoObject* CreateYesNoScreen(const FText& InScreenTitle, const FText& InScreenMessage);
	static UConfirmScreenInfoObject* CreateOkCancelScreen(const FText& InScreenTitle, const FText& InScreenMessage);

	UPROPERTY(Transient)
	FText ScreenTitle;

	UPROPERTY(Transient)
	FText ScreenMessage;

	UPROPERTY(Transient)
	TArray<FConfirmScreenButtonInfo> AvailableScreenButtons;

};

/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class ADVANCEDRPG_API UWidget_ConfirmScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

public:
	//Get's Called outside of the class, when Widget is  constructed and before it is pushed into the modal stack. This is where we will set up the widget based on the info object passed in.
	void InitConfrimScreen(UConfirmScreenInfoObject* InConfirmScreenInfoObject,TFunction<void(EConfirmScreenButtonType)> ClickedButtonCallBack);




private:
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* CommonTextBlock_Title;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* CommonTextBlock_Message;

	UPROPERTY(meta = (BindWidget))
	UDynamicEntryBox* DynamicEntryBox_Buttons;


	
};
