// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"

#include "WarriorHeroController.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API AWarriorHeroController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public :
	AWarriorHeroController();

	virtual FGenericTeamId GetGenericTeamId() const override;

private:

	FGenericTeamId HeroTeamId;
};
