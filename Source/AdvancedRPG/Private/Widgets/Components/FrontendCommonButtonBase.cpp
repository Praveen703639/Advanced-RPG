// pravin's  patashala all Rights Reserved 


#include "Widgets/Components/FrontendCommonButtonBase.h"
#include "CommonTextBlock.h"
#include "Subsystems/FrontEndUISubsystem.h"

void UFrontendCommonButtonBase::SetButtonText(FText InText)
{
	if (CommonTextBlock_ButtonText && !InText.IsEmpty())
	{
		CommonTextBlock_ButtonText->SetText(bUserUpperCaseForButtonText ? InText.ToUpper() : InText);
	}
}

void UFrontendCommonButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetButtonText(ButtonDisplayText);
}

void UFrontendCommonButtonBase::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();
	if (CommonTextBlock_ButtonText && GetCurrentTextStyleClass())
	{
		CommonTextBlock_ButtonText->SetStyle(GetCurrentTextStyleClass());
	}
}

void UFrontendCommonButtonBase::NativeOnHovered()
{
	Super::NativeOnHovered();
	if (!ButtonDescriptionText.IsEmpty())
	{
		UFrontEndUISubsystem* UISubsystem = UFrontEndUISubsystem::Get(this);
		if (UISubsystem)
		{
			UISubsystem->OnButtonDescriptionTextUpdatedDelegate.Broadcast(this, ButtonDescriptionText);
			UE_LOG(LogTemp, Warning, TEXT("✅ Button hovered: Broadcast description text: %s"), *ButtonDescriptionText.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("❌ NativeOnHovered FAILED: UISubsystem is nullptr! Button: %s"), *GetName());
		}
	}
}

void UFrontendCommonButtonBase::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();
	UFrontEndUISubsystem* UISubsystem = UFrontEndUISubsystem::Get(this);
	if (UISubsystem)
	{
		UISubsystem->OnButtonDescriptionTextUpdatedDelegate.Broadcast(this, FText::GetEmpty());
		UE_LOG(LogTemp, Warning, TEXT("✅ Button unhovered: Description cleared"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ NativeOnUnhovered FAILED: UISubsystem is nullptr!"));
	}
}
