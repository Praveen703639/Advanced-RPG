// pravin's  patashala all Rights Reserved 


#include "AsyncActions/AsyncAction_PushConfirmScreen.h"
#include "Subsystems/FrontEndUISubsystem.h"

UAsyncAction_PushConfirmScreen* UAsyncAction_PushConfirmScreen::PushConfirmScreen(const UObject* WorldContextObject, EConfirmScreenType InScreenType, FText InScreenTitle, FText InScreenMessage)
{
	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			UAsyncAction_PushConfirmScreen* Node  = NewObject<UAsyncAction_PushConfirmScreen>();
			Node->CachedOwningWorld = World;
			Node->CachedScreenType = InScreenType;
			Node->CachedScreenTitle = InScreenTitle;
			Node->CachedScreenMessage = InScreenMessage;

			Node->RegisterWithGameInstance(World);
			return Node;
		}
	}
	return nullptr;
}

void UAsyncAction_PushConfirmScreen::Activate()
{
	UFrontEndUISubsystem::Get(CachedOwningWorld.Get())->PushConfirmScreenToModalStackAsync(
		CachedScreenType, 
		CachedScreenTitle, 
		CachedScreenMessage,
		[this](EConfirmScreenButtonType ClickedButtonType)
		{
			OnButtonClicked.Broadcast(ClickedButtonType);
			SetReadyToDestroy();
		}
	);
}
