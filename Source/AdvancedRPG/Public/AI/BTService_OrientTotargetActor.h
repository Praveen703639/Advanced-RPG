// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_OrientTotargetActor.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UBTService_OrientTotargetActor : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_OrientTotargetActor();

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector InTargetActorKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	float RotationInterpSpeed;


};
