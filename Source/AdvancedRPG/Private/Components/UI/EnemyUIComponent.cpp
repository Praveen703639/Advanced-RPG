// pravin's  patashala all Rights Reserved 


#include "Components/UI/EnemyUIComponent.h"
#include "Widgets/InGameWidgets/WarriorWidgetBase.h"

void UEnemyUIComponent::RegisterEnemyDrawnWidget(UWarriorWidgetBase* InWidgetToRegister)
{
	EnemyDrawnWidgets.Add(InWidgetToRegister);
}

void UEnemyUIComponent::RemoveEnemyDrawnWidgetsIfAny()
{
	if (EnemyDrawnWidgets.IsEmpty())
	{
		return;
	}

	for (UWarriorWidgetBase* DrawnWidget : EnemyDrawnWidgets)
	{
		if (DrawnWidget)
		{
			DrawnWidget->RemoveFromParent();
		}
	}


	EnemyDrawnWidgets.Empty();


}
