// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "WarriorTypes/WarriorEnumTypes.h"

#include "WarriorSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UWarriorSaveGame : public USaveGame
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly)
	EWarriorGameDifficulty SavedCurrentGameDifficulty;
	
};
