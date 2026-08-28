// pravin's  patashala all Rights Reserved 


#include "FrontendtFunctionLibrary.h"
#include "FrontendSettings/FrontEndDeveloperSettings.h"

TSoftClassPtr<UWidget_ActivatableBase> UFrontendtFunctionLibrary::GetFrontEndSoftWidgetClassByTag(UPARAM(meta = (Categories = " FrontEnd.Widget ")) FGameplayTag InWidgetTag)
{
	const UFrontEndDeveloperSettings* FrontEndDeveloperSettings =  GetDefault<UFrontEndDeveloperSettings>();
	checkf(FrontEndDeveloperSettings->FrontEndWidgetMap.Find(InWidgetTag),TEXT("Could not find the corresponding widget with the tag %s"), *InWidgetTag.ToString())
	return FrontEndDeveloperSettings->FrontEndWidgetMap.FindRef(InWidgetTag);
	
}
