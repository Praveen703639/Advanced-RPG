// pravin's  patashala all Rights Reserved 


#include "Widgets/Widget_ActivatableBase.h"
#include "Controllers/FrontEndController.h"

AFrontEndController* UWidget_ActivatableBase::GetOwningFrontEndController()
{
	if (!CachedFrontEndOwningPC.IsValid())
	{
		CachedFrontEndOwningPC = (GetOwningPlayer<AFrontEndController>());
	}

	return CachedFrontEndOwningPC.IsValid() ? CachedFrontEndOwningPC.Get(): nullptr;
}