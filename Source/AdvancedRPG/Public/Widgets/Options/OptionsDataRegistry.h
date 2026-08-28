// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OptionsDataRegistry.generated.h"

class UListDataObject_Collection;
/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UOptionsDataRegistry : public UObject
{
	GENERATED_BODY()

public:

	// gets called by options screen right after the object of type UOptionsDataRegistry is created and initialized
	void InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer);
	const TArray<UListDataObject_Collection*>& GetRegisteredOptionsTabCollections() const { return RegisteredOptionsTabCollections; }


private:

	void InitGameplayCollectionTab();

	void InitAudioCollectionTab();
	void InitVideoCollectionTab();
	void InitControlsCollectionTab();

	UPROPERTY(Transient)
	TArray<UListDataObject_Collection*> RegisteredOptionsTabCollections;


	
};
