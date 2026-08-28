// pravin's  patashala all Rights Reserved 


#include "AI/BTT_RotateToFaceTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

UBTT_RotateToFaceTarget::UBTT_RotateToFaceTarget()
{
	NodeName = TEXT("Native Rotate To Face Target");
	AnglePrecision = 10.f;
	RotationInterpSpeed = 5.f;

	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;
	INIT_TASK_NODE_NOTIFY_FLAGS();
	InTargetKeyToFaceKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_RotateToFaceTarget, InTargetKeyToFaceKey), AActor::StaticClass());

}

void UBTT_RotateToFaceTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		InTargetKeyToFaceKey.ResolveSelectedKey(*BBAsset);
	}
}

FString UBTT_RotateToFaceTarget::GetStaticDescription() const
{
	const FString KeyDescription = InTargetKeyToFaceKey.SelectedKeyName.ToString();
	return FString::Printf(TEXT("Smoothly Rotates to face %s key until the precision  : %s is reached"), *KeyDescription, *FString::SanitizeFloat(AnglePrecision));
	
}

void UBTT_RotateToFaceTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FRotateToFaceTargetMemory* Memory = CastInstanceNodeMemory<FRotateToFaceTargetMemory>(NodeMemory);

	if(!Memory || !Memory->IsValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	if (HasreachedAnglePrecision(Memory->OwningPawn.Get(), Memory->TargetActor.Get()))
	{
		Memory->Reset();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else
	{
		const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Memory->OwningPawn->GetActorLocation(), Memory->TargetActor->GetActorLocation());
		const FRotator TargetRot = UKismetMathLibrary::RInterpTo(Memory->OwningPawn->GetActorRotation(), LookAtRot, DeltaSeconds, RotationInterpSpeed);
		Memory->OwningPawn->SetActorRotation(TargetRot);
	
	}

}

EBTNodeResult::Type UBTT_RotateToFaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetKeyToFaceKey.SelectedKeyName);

	AActor* TargetActor = Cast<AActor>(ActorObject);
	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

	FRotateToFaceTargetMemory* Memory = CastInstanceNodeMemory<FRotateToFaceTargetMemory>(NodeMemory);
	check(Memory);
	Memory->OwningPawn = OwningPawn;
	Memory->TargetActor = TargetActor;

	if (!Memory->IsValid())
	{
		return EBTNodeResult::Failed;
	}

	if (HasreachedAnglePrecision(OwningPawn, TargetActor))
	{
		Memory->Reset();
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::InProgress;

}

bool UBTT_RotateToFaceTarget::HasreachedAnglePrecision(APawn* QueryActor, AActor* TargetActor) const
{
	const FVector ownerForward =  QueryActor->GetActorLocation();
	const FVector OwnerToTargetNormalized = (TargetActor->GetActorLocation() - ownerForward).GetSafeNormal();
	const float DotResult = FVector::DotProduct(ownerForward, OwnerToTargetNormalized);
	const float AngleDiff = UKismetMathLibrary::DegAcos(DotResult);

	return AngleDiff <= AnglePrecision;

}

uint16 UBTT_RotateToFaceTarget::GetInstanceMemorySize() const
{
	return sizeof(FRotateToFaceTargetMemory);
}