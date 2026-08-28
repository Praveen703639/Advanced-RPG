// pravin's  patashala all Rights Reserved 


#include "Controllers/FrontEndController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"

void AFrontEndController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);

	// Only change view for the local player controller (prevents server/non-local controllers from changing view)
	if (!IsLocalController())
	{
		UE_LOG(LogTemp, Verbose, TEXT("AFrontEndController::OnPossess - not a local controller, skipping view change"));
		return;
	}

	TArray<AActor*> FoundCameras;
	// Use GetAllActorsOfClass with a world context and then filter by tag.
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), FoundCameras);

	UE_LOG(LogTemp, Log, TEXT("AFrontEndController::OnPossess - found %d camera actors"), FoundCameras.Num());

	for (AActor* Cam : FoundCameras)
	{
		if (Cam && Cam->ActorHasTag(FName("Default")))
		{
			// Use a blend for a smooth transition; 0.f would be instant.
			SetViewTargetWithBlend(Cam, 0.25f);
			UE_LOG(LogTemp, Log, TEXT("AFrontEndController::OnPossess - set view to camera: %s"), *Cam->GetName());
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("AFrontEndController::OnPossess - no camera actor with tag 'Default' found"));
}
