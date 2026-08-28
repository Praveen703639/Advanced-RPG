// pravin's  patashala all Rights Reserved 


#include "Widgets/Widget_ConfirmScreen.h"
#include "CommonTextBlock.h"
#include "Components/DynamicEntryBox.h"
#include "Widgets/Components/FrontendCommonButtonBase.h"
#include "ICommonInputModule.h"

UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateOkScreen(const FText& InScreenTitle, const FText& InScreenMessage)
{
	UConfirmScreenInfoObject* InfoObject = NewObject<UConfirmScreenInfoObject>();

	InfoObject->ScreenTitle = InScreenTitle;
	InfoObject->ScreenMessage = InScreenMessage;

	FConfirmScreenButtonInfo OKButtonInfo;
	OKButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Closed;
	OKButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("OK"));

	InfoObject->AvailableScreenButtons.Add(OKButtonInfo);


	return InfoObject;
}

UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateYesNoScreen(const FText& InScreenTitle, const FText& InScreenMessage)
{
	UConfirmScreenInfoObject* InfoObject = NewObject<UConfirmScreenInfoObject>();

	InfoObject->ScreenTitle = InScreenTitle;
	InfoObject->ScreenMessage = InScreenMessage;

	FConfirmScreenButtonInfo YesButtonInfo;
	YesButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Confirmed;
	YesButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("Yes"));

	FConfirmScreenButtonInfo NoButtonInfo;
	NoButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::cancelled;
	NoButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("No"));

	InfoObject->AvailableScreenButtons.Add(YesButtonInfo);
	InfoObject->AvailableScreenButtons.Add(NoButtonInfo);


	return InfoObject;
}

UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateOkCancelScreen(const FText& InScreenTitle, const FText& InScreenMessage)
{
	UConfirmScreenInfoObject* InfoObject = NewObject<UConfirmScreenInfoObject>();

	InfoObject->ScreenTitle = InScreenTitle;
	InfoObject->ScreenMessage = InScreenMessage;

	FConfirmScreenButtonInfo OkButtonInfo;
	OkButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Confirmed;
	OkButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("Ok"));

	FConfirmScreenButtonInfo CancelButtonInfo;
	CancelButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::cancelled;
	CancelButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("Cancel"));

	InfoObject->AvailableScreenButtons.Add(OkButtonInfo);
	InfoObject->AvailableScreenButtons.Add(CancelButtonInfo);


	return InfoObject;
}

void UWidget_ConfirmScreen::InitConfrimScreen(UConfirmScreenInfoObject* InConfirmScreenInfoObject, TFunction<void(EConfirmScreenButtonType)> ClickedButtonCallBack)
{
	check(InConfirmScreenInfoObject && CommonTextBlock_Title && CommonTextBlock_Message && DynamicEntryBox_Buttons);
	CommonTextBlock_Title->SetText(InConfirmScreenInfoObject->ScreenTitle);
	CommonTextBlock_Title->SetText(InConfirmScreenInfoObject->ScreenMessage);
	// checking if entry box has created old buttons previously
	if (DynamicEntryBox_Buttons->GetNumEntries() != 0)
	{
		// Clearing old buttons if they exist, before creating new ones
		//the widget type for entry box is specified in the child widget blueprint
		DynamicEntryBox_Buttons->Reset<UFrontendCommonButtonBase>(
			[](UFrontendCommonButtonBase& ExistingButton)
			{
				ExistingButton.OnClicked().Clear();
			}
		
		);
	}
	check(!InConfirmScreenInfoObject->AvailableScreenButtons.IsEmpty());
	for (const FConfirmScreenButtonInfo& AvailableButtonInfo : InConfirmScreenInfoObject->AvailableScreenButtons)
	{
		FDataTableRowHandle InputActionRowHandle;

		switch (AvailableButtonInfo.ConfirmScreenButtonType)
		{
		case EConfirmScreenButtonType::Confirmed:
			InputActionRowHandle = ICommonInputModule::GetSettings().GetDefaultClickAction();
			break;

		case EConfirmScreenButtonType::cancelled:
			InputActionRowHandle = ICommonInputModule::GetSettings().GetDefaultBackAction();
			break;

		case EConfirmScreenButtonType::Closed:
			InputActionRowHandle = ICommonInputModule::GetSettings().GetDefaultBackAction();
			break;

		default:
			break;
		}

		UFrontendCommonButtonBase* AddedButton = DynamicEntryBox_Buttons->CreateEntry<UFrontendCommonButtonBase>();
		AddedButton->SetButtonText(AvailableButtonInfo.ButtonTextToDisplay);
		AddedButton->SetTriggeredInputAction(InputActionRowHandle);
		AddedButton->OnClicked().AddLambda(
			[ClickedButtonCallBack, AvailableButtonInfo,this]()
			{
				ClickedButtonCallBack(AvailableButtonInfo.ConfirmScreenButtonType);
				DeactivateWidget();
			}
		);


	}


	if (DynamicEntryBox_Buttons->GetNumEntries() != 0)
	{
		// set focus to last button added,so if there are two buttons one is yes and other is no,
		// our gamepad or input mouse or keyboard focus on no button
		(DynamicEntryBox_Buttons->GetAllEntries().Last()->SetFocus());
	}
}
