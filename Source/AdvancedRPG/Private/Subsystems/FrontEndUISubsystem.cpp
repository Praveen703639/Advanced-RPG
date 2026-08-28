// pravin's  patashala all Rights Reserved 


#include "Subsystems/FrontEndUISubsystem.h"
#include "WarriorDebugHelper.h"
#include "Engine/AssetManager.h"
#include "Widgets/Widget_PrimaryLayOut.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widgets/Widget_ConfirmScreen.h"
#include "FrontEndGameplayTags.h"
#include "FrontendtFunctionLibrary.h"



UFrontEndUISubsystem* UFrontEndUISubsystem::Get(UObject* WorldContextObject)
{
	if (GEngine)
	{
        UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
		if (!World)
		{
			UE_LOG(LogTemp, Warning, TEXT("UFrontEndUISubsystem::Get - WorldContextObject yielded null world"));
			return nullptr;
		}

		UGameInstance* GI = World->GetGameInstance();
		if (!GI)
		{
			UE_LOG(LogTemp, Warning, TEXT("UFrontEndUISubsystem::Get - World has no GameInstance"));
			return nullptr;
		}

		return UGameInstance::GetSubsystem<UFrontEndUISubsystem>(GI);
	}
	return nullptr;
}

bool UFrontEndUISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    // Only create this subsystem on non-dedicated server instances (i.e., client or listen server)
	UGameInstance* GI = CastChecked<UGameInstance>(Outer);
	return (GI && !GI->IsDedicatedServerInstance());
}

void UFrontEndUISubsystem::RegisterCretaedPrimaryLayout(UWidget_PrimaryLayOut* CreatedLayout)
{
	check(CreatedLayout);
	CreatedPrimaryLayout = CreatedLayout;
	

}

void UFrontEndUISubsystem::PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag, TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass, TFunction<void(EAsynPushWidgetState, UWidget_ActivatableBase*)> AsyncPushStateCallback)
{
	check(!InSoftWidgetClass.IsNull());

	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		InSoftWidgetClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(

			[InSoftWidgetClass, this, InWidgetStackTag, AsyncPushStateCallback]()
			{
				UClass* LoadedWidgetClass = InSoftWidgetClass.Get();

				check(LoadedWidgetClass && CreatedPrimaryLayout);

				UCommonActivatableWidgetContainerBase*  FoundWidgetStack =CreatedPrimaryLayout->FindWidgetStackByTag(InWidgetStackTag);
				UWidget_ActivatableBase* CreatedWidget = FoundWidgetStack->AddWidget<UWidget_ActivatableBase>(
					LoadedWidgetClass,
					[AsyncPushStateCallback](UWidget_ActivatableBase& CreatedWidgetInstance)
					{
						AsyncPushStateCallback(EAsynPushWidgetState::OnCreatedBeforePush,&CreatedWidgetInstance);
					}
				);

				AsyncPushStateCallback(EAsynPushWidgetState::AfetrPush, CreatedWidget);

			
			}

		)

	);

}

void UFrontEndUISubsystem::PushConfirmScreenToModalStackAsync(EConfirmScreenType InScreenType, const FText& InScreenTitle, const FText& InScreenMessage, TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallBack)
{
	UConfirmScreenInfoObject* ConfirmScreenInfoObject = nullptr;
	switch (InScreenType)
	{
	case EConfirmScreenType::Ok:
		ConfirmScreenInfoObject = UConfirmScreenInfoObject::CreateOkScreen(InScreenTitle, InScreenMessage);
		break;
		
	case EConfirmScreenType::YesNo:
		ConfirmScreenInfoObject = UConfirmScreenInfoObject::CreateYesNoScreen(InScreenTitle, InScreenMessage);
		break;

	case EConfirmScreenType::OkCancel:
		ConfirmScreenInfoObject = UConfirmScreenInfoObject::CreateOkCancelScreen(InScreenTitle, InScreenMessage);
		break;

	case EConfirmScreenType::Unknown:
		break;

	default:
		break;
	}
	check(ConfirmScreenInfoObject);

	PushSoftWidgetToStackAsync(
		FGameplayTags::FrontEnd_WidgetStack_Modal,
		UFrontendtFunctionLibrary::GetFrontEndSoftWidgetClassByTag(FGameplayTags::FrontEnd_Widget_ConfirmScreen),
		[ConfirmScreenInfoObject, ButtonClickedCallBack](EAsynPushWidgetState InPushState, UWidget_ActivatableBase* PushedWidget)
		{
			if (InPushState == EAsynPushWidgetState::OnCreatedBeforePush)
			{
				UWidget_ConfirmScreen* CreatedConfirmScreen = CastChecked<UWidget_ConfirmScreen>(PushedWidget);
				CreatedConfirmScreen->InitConfrimScreen(ConfirmScreenInfoObject, ButtonClickedCallBack);
			}
		}
	);
}
