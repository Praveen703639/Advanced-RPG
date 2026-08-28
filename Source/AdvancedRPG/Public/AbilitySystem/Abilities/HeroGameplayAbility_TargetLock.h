// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGameplayAbility_TargetLock.generated.h"

class UWarriorWidgetBase;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UHeroGameplayAbility_TargetLock : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, Category = "Target Lock")
	void OnTargetLockTick(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void  SwitchTarget(const FGameplayTag& InSwitchDirectionTag);

private:
	void TryLockOnTarget();
	void GetAvailableTargetsToLock();
	AActor* GetNearestTargetFromAvailableTargetActors(const TArray<AActor*>& InAvailableActors);
	void GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft, TArray<AActor*>& OutActorsOnRight);

	void DrawTargetLockWidget();
	void InitTargetLockMovement();
	void InitTargetLockMappingContext();

	void CancelTargetLockAbility();
	void CleanUp();
	void SetTargetLockWidgetPosition();
	void ResetTargetLockMovement();	
	void ResetTargetLockMappingContext();



	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	float TargetLockRange = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	FVector TargetLockBoxExtent = FVector(5000.f, 5000.f, 300.f);

	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	TArray< TEnumAsByte < EObjectTypeQuery > > BoxTraceChannel;

	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	bool bShowPersistentDebugShape = false;

	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	TSubclassOf<UWarriorWidgetBase> TargetLockWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	float TargetLockRotationInterpSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	float TargetLockMaxWalkSpeedModifier = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	UInputMappingContext* TargetLockInputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	float TargetLockCameraOffsetDistance = 20.f;

	UPROPERTY()
	TArray<AActor*> AvailableActorsToLock;

	UPROPERTY()
	AActor* CurrentLockedTarget;

	UPROPERTY()
	UWarriorWidgetBase* DrawnTargetLockWidget;

	UPROPERTY()
	FVector2D TargetLockWidgetSize = FVector2D::ZeroVector;
		
	UPROPERTY()
	float CachedDefaultMaxWalkSpeed = 0.f;
};
