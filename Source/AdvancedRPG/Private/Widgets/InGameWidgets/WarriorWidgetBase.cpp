// pravin's  patashala all Rights Reserved 


#include "Widgets/InGameWidgets/WarriorWidgetBase.h"
#include "Interfaces/PawnUIInterface.h"
#include "WarriorDebugHelper.h"
#include "WarriorDebugHelper.h"


void UWarriorWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(GetOwningPlayerPawn()))
	{
		if (UHeroUIComponent* HeroUIComponent = PawnUIInterface->GetHeroUIComponent())
		{
			BP_OnOwningHeroUIComponentInitialized(HeroUIComponent);
		}
	}
}

void UWarriorWidgetBase::Debug_OnHealthPercent(float Percent)
{
	const FString DebugString = FString::Printf(TEXT("Widget received Health Percent: %f"), Percent);
	
}



void UWarriorWidgetBase::InitEnemyCreatedWidget(AActor* OwningEnemyActor)
{
	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(OwningEnemyActor))
	{
		UEnemyUIComponent* EnemyUIComponent = PawnUIInterface->GetEnemyUIComponent();

		checkf(EnemyUIComponent, TEXT("Failed to extract EnemyUIComponent from %s : "), *OwningEnemyActor->GetActorNameOrLabel());
		BP_OnOwningEnemyUIComponentInitialized(EnemyUIComponent);
	}
}

