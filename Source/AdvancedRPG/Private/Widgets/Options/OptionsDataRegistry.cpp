// pravin's  patashala all Rights Reserved 


#include "Widgets/Options/OptionsDataRegistry.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"

void UOptionsDataRegistry::InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer)
{
	InitGameplayCollectionTab();
	InitAudioCollectionTab();
	InitVideoCollectionTab();
	InitControlsCollectionTab();
}

void UOptionsDataRegistry::InitGameplayCollectionTab()
{
	UListDataObject_Collection* GameplayTabCollection = NewObject<UListDataObject_Collection>();
	GameplayTabCollection->SetDataID(FName(TEXT("GameplayTabCollection")));
	GameplayTabCollection->SetDataDisplayName(FText::FromString(TEXT("Audio")));

	RegisteredOptionsTabCollections.Add(GameplayTabCollection);

}

void UOptionsDataRegistry::InitAudioCollectionTab()
{
	UListDataObject_Collection* AudioTabCollection = NewObject<UListDataObject_Collection>();
	AudioTabCollection->SetDataID(FName(TEXT("AudioTabCollection")));
	AudioTabCollection->SetDataDisplayName(FText::FromString(TEXT("AudioTabCollection")));

	RegisteredOptionsTabCollections.Add(AudioTabCollection);
}

void UOptionsDataRegistry::InitVideoCollectionTab()
{
	UListDataObject_Collection* VideoTabCollection = NewObject<UListDataObject_Collection>();
	VideoTabCollection->SetDataID(FName(TEXT("VideoTabCollection")));
	VideoTabCollection->SetDataDisplayName(FText::FromString(TEXT("VideoTabCollection")));

	RegisteredOptionsTabCollections.Add(VideoTabCollection);
}

void UOptionsDataRegistry::InitControlsCollectionTab()
{
	UListDataObject_Collection* ControlsTabCollection = NewObject<UListDataObject_Collection>();
	ControlsTabCollection->SetDataID(FName(TEXT("ControlsTabCollection")));
	ControlsTabCollection->SetDataDisplayName(FText::FromString(TEXT("ControlsTabCollection")));

	RegisteredOptionsTabCollections.Add(ControlsTabCollection);
}
