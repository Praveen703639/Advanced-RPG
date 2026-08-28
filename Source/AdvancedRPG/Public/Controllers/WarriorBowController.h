// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "WarriorBowController.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API AWarriorBowController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AWarriorBowController();

	virtual FGenericTeamId GetGenericTeamId() const override;

private:

	FGenericTeamId BowTeamId;
};