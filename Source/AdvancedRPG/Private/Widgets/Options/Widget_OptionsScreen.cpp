// pravin's  patashala all Rights Reserved 


#include "Widgets/Options/Widget_OptionsScreen.h"
#include  "Input/CommonUIInputTypes.h"
#include "ICommonInputModule.h"
#include "Widgets/Options/OptionsDataRegistry.h"
#include "Widgets/Components/FrontEndTabListWidgetBase.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"


#include "WarriorDebugHelper.h"


void UWidget_OptionsScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!ResetAction.IsNull())
	{
		ResetActionHandle = RegisterUIActionBinding(
			FBindUIActionArgs(
				ResetAction,
				true,
				FSimpleDelegate::CreateUObject(this, &ThisClass::OnResetBoundActionTriggered)

			)


		);


	}

	RegisterUIActionBinding(
		FBindUIActionArgs(
			ICommonInputModule::GetSettings().GetDefaultBackAction(),
			true,
			FSimpleDelegate::CreateUObject(this, &ThisClass::OnBackBoundActionTriggered)
		)
	);

}

void UWidget_OptionsScreen::NativeOnActivated()
{
	Super::NativeOnActivated();

	for (UListDataObject_Collection* TabCollection : GetOrCreateDataRegistry()->GetRegisteredOptionsTabCollections())
	{
		if (!TabCollection)
		{
			continue;
		}
		const FName TabID = TabCollection->GetDataID();

		if (TabListWidget_OptionsTab->GetTabButtonBaseByID(TabID) != nullptr)
		{
			continue;
		}

		TabListWidget_OptionsTab->RequestRegisterTab(TabID, TabCollection->GetDataDisplayName());

	}
}

UOptionsDataRegistry* UWidget_OptionsScreen::GetOrCreateDataRegistry()
{
	if (!OwningDataRegistry)
	{
		OwningDataRegistry = NewObject<UOptionsDataRegistry>();
		OwningDataRegistry->InitOptionsDataRegistry(GetOwningLocalPlayer());
	}

	checkf(OwningDataRegistry, TEXT("Failed to create Options Data Registry for Options Screen Widget %s"), *GetName());
	return OwningDataRegistry;
}

void UWidget_OptionsScreen::OnResetBoundActionTriggered()
{
	Debug::Print(TEXT("Reset Bound Action Triggered :")); 

}

void UWidget_OptionsScreen::OnBackBoundActionTriggered()
{
	Debug::Print(TEXT("Back Bound Action Triggered :"));

	DeactivateWidget();
}
