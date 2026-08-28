// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_RotateToFaceTarget.generated.h"

struct FRotateToFaceTargetMemory
{
	TWeakObjectPtr<APawn> OwningPawn;
	TWeakObjectPtr<AActor> TargetActor;

	bool IsValid() const
	{
		return OwningPawn.IsValid() && TargetActor.IsValid();
	}

	void Reset()
	{
		OwningPawn.Reset();
		TargetActor.Reset();
	}
};


/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UBTT_RotateToFaceTarget : public UBTTaskNode
{
	GENERATED_BODY()

	UBTT_RotateToFaceTarget();

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual FString GetStaticDescription() const override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;

	bool HasreachedAnglePrecision(APawn* QueryActor, AActor* TargetActor) const;

	UPROPERTY(EditAnywhere, Category = "Face Target")
	float AnglePrecision;

	UPROPERTY(EditAnywhere, Category = "Face Target")
	float RotationInterpSpeed;

	UPROPERTY(EditAnywhere, Category = "Face Target")
	FBlackboardKeySelector InTargetKeyToFaceKey;

};
