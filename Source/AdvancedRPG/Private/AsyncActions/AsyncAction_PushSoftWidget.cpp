// pravin's  patashala all Rights Reserved 


#include "AsyncActions/AsyncAction_PushSoftWidget.h"
#include "Subsystems/FrontEndUISubsystem.h"
#include "Widgets/Widget_ActivatableBase.h"

UAsyncAction_PushSoftWidget* UAsyncAction_PushSoftWidget::PushSoftWidget(const UObject* WorldContextObject, APlayerController* OwningPlayerController, TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass, UPARAM(meta = (Categories = "FrontEnd.WidgetStack")) FGameplayTag InWidgetStackTag, bool bFocusOnNwelyPushedWidget)
{
	checkf(!InSoftWidgetClass.IsNull(),TEXT("PushSoftWidgetToStack was passed a null soft widget class "));
	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			UAsyncAction_PushSoftWidget* Node  = NewObject<UAsyncAction_PushSoftWidget>();
			Node->RegisterWithGameInstance(World);
			Node->CachedOwningWorld = World;
			Node->CachedOwningPC = OwningPlayerController;
			Node->CachedSoftWidgetClass = InSoftWidgetClass;
			Node->CachedWidgetStackTag = InWidgetStackTag;
			Node->bCachedFocusOnNwelyPushedWidget = bFocusOnNwelyPushedWidget;
			return Node;
		}
	}
	return nullptr;
}

void UAsyncAction_PushSoftWidget::Activate()
{
	UFrontEndUISubsystem* FrontEndUISubsystem = UFrontEndUISubsystem::Get(CachedOwningWorld.Get());
	FrontEndUISubsystem->PushSoftWidgetToStackAsync(CachedWidgetStackTag, CachedSoftWidgetClass,
		[this](EAsynPushWidgetState InPushState, UWidget_ActivatableBase* PushedWidget)
		{
			switch (InPushState)
			{
			case EAsynPushWidgetState::OnCreatedBeforePush:

				PushedWidget->SetOwningPlayer(CachedOwningPC.Get());
				OnWidgetCreatedBeforePush.Broadcast(PushedWidget);

				break;

			case EAsynPushWidgetState::AfetrPush:
				AfterPush.Broadcast(PushedWidget);
				if (bCachedFocusOnNwelyPushedWidget)
				{
					if (UWidget* WidgetToFocus = PushedWidget->GetDesiredFocusTarget())
					{
						WidgetToFocus->SetFocus();
					}
				}

				SetReadyToDestroy();
				break;
			default:
				break;
			}
		}

	);
}
