// pravin's  patashala all Rights Reserved 


#include "Widgets/Widget_PrimaryLayOut.h"
#include "WarriorDebugHelper.h"

UCommonActivatableWidgetContainerBase* UWidget_PrimaryLayOut::FindWidgetStackByTag(const FGameplayTag& InTag) const
{
	checkf(RegisteredWidgetStackMap.Contains(InTag), TEXT("cannot find the widget stack by the Tag %s: "), *InTag.ToString());
	
	return RegisteredWidgetStackMap.FindRef(InTag);

}

void UWidget_PrimaryLayOut::RegisterWidgetStack(UPARAM(meta = (Categories = "FrontEnd.WidgetStack")) FGameplayTag InstackTag, UCommonActivatableWidgetContainerBase* InStack)
{
	if (!IsDesignTime())
	{
		if (!RegisteredWidgetStackMap.Contains(InstackTag)) {

			RegisteredWidgetStackMap.Add(InstackTag, InStack);
			

		}
	}
}
