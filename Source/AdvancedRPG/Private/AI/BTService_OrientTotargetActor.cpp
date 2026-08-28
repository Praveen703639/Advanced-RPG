// pravin's  patashala all Rights Reserved 


#include "AI/BTService_OrientTotargetActor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

UBTService_OrientTotargetActor::UBTService_OrientTotargetActor()
{
	NodeName = TEXT(" Native Orient To Target Actor");

	INIT_SERVICE_NODE_NOTIFY_FLAGS();

	RotationInterpSpeed = 5.f;
	Interval = 0.f;
	RandomDeviation = 0.f;
	InTargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_OrientTotargetActor, InTargetActorKey), AActor::StaticClass());



}

void UBTService_OrientTotargetActor::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		InTargetActorKey.ResolveSelectedKey(*BBAsset);
	}
}

FString UBTService_OrientTotargetActor::GetStaticDescription() const
{
	const FString& KeyDescription = InTargetActorKey.SelectedKeyName.ToString();
	return  FString::Printf(TEXT("Orient Rotion To %s Key %s "), *Super::GetStaticDescription(), *GetStaticServiceDescription());

	
}

void UBTService_OrientTotargetActor::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetActorKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(ActorObject);

	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (OwningPawn && TargetActor)
	{
		const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(OwningPawn->GetActorLocation(), TargetActor->GetActorLocation());
		const FRotator TargetRot = FMath::RInterpTo(OwningPawn->GetActorRotation(), LookAtRot, DeltaSeconds, RotationInterpSpeed);
	
		OwningPawn->SetActorRotation(TargetRot);
	}


}
