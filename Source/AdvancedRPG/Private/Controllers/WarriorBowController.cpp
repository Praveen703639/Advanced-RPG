// pravin's  patashala all Rights Reserved 


#include "Controllers/WarriorBowController.h"

AWarriorBowController::AWarriorBowController()
{
	BowTeamId = FGenericTeamId(0);
}

FGenericTeamId AWarriorBowController::GetGenericTeamId() const
{
	return BowTeamId;
}