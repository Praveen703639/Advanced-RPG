// pravin's patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGameplayAbility_ShootProj.generated.h"

class UWarriorWidgetBase;
class UInputMappingContext;

UCLASS()
class ADVANCEDRPG_API UHeroGameplayAbility_ShootProj : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()

public:
	// Called every frame while aiming (wire to Event Tick in ability BP)
	UFUNCTION(BlueprintCallable, Category = "Aim")
	void OnAimTick(float DeltaTime);

	// Called from your Input Action (Axis2D) in the Aim Mapping Context
	UFUNCTION(BlueprintCallable, Category = "Aim")
	void MoveAim(const FVector2D& InInputDirection);

	// Returns the world location the aim reticle is currently targeting.
	// Use this in your Ability BP to know where to spawn/aim your projectile.
	UFUNCTION(BlueprintPure, Category = "Aim")
	FVector GetAimTargetLocation() const;

	// Call from BP when you want to cleanly exit aim mode (e.g. toggle N pressed again, or after shooting)
	UFUNCTION(BlueprintCallable, Category = "Aim")
	void FinishAiming();

	// Call from BP if you need to cancel aim mode (e.g. player got stunned / died)
	UFUNCTION(BlueprintCallable, Category = "Aim")
	void CancelAiming();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	// --- AIM PHASE ---
	void InitAimMappingContext();
	void ResetAimMappingContext();
	void DrawAimWidget();
	void SetAimWidgetPosition();
	void UpdateAimLocation(float DeltaTime);
	void CleanUpAimPhase();

	// ========== PROPERTIES ==========

	// Widget to draw at the aim location (reticle / ground marker).
	// For an arrow-worthy look, use a UMG with a central dot + outer ring + downward arrow.
	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	TSubclassOf<UWarriorWidgetBase> AimWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	UInputMappingContext* AimInputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float AimMoveSpeed = 1200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float AimMinDistance = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float AimMaxDistance = 8000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float AimGroundTraceDepth = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	bool bShowDebugTrace = false;

	// ========== RUNTIME STATE ==========

	UPROPERTY()
	FVector2D CurrentAimInput = FVector2D::ZeroVector;

	// World location of the aim reticle. Updated every tick. Ground-traced.
	// In your Ability BP, you can drag from Self and read this directly.
	UPROPERTY(BlueprintReadOnly, Category = "Aim", meta = (AllowPrivateAccess = "true"))
	FVector CurrentAimWorldLocation = FVector::ZeroVector;

	UPROPERTY()
	UWarriorWidgetBase* DrawnAimWidget = nullptr;

	UPROPERTY()
	FVector2D AimWidgetSize = FVector2D::ZeroVector;
};