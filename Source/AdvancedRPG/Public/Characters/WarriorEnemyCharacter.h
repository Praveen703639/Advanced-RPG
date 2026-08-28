// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "WarriorEnemyCharacter.generated.h"

class UEnemyCombatComponent;
class UEnemyUIComponent;
class UWidgetComponent;
class UCapsuleComponent;

UCLASS()
class ADVANCEDRPG_API AWarriorEnemyCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()

public:
	AWarriorEnemyCharacter();

	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;

	// === DECLARE these functions that are defined in CPP ===
	virtual UPawnUIComponents* GetPawnUIComponents() const override;
	UEnemyUIComponent* GetEnemyUIComponent() const;

	// === BOSS FLAG ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	bool bIsBoss = false;

	// Called when frozen — stores VFX for later cleanup
	UFUNCTION(BlueprintCallable, Category = "Freeze")
	void AddFreezeVFX(UParticleSystemComponent* VFXComponent);

	// Called when unfrozen — destroys all freeze VFX
	UFUNCTION(BlueprintCallable, Category = "Freeze")
	void ClearFreezeVFX();

	// Check if currently frozen
	UFUNCTION(BlueprintPure, Category = "Freeze")
	bool IsFrozen() const { return bIsFrozen; }

	UPROPERTY(BlueprintReadWrite, Category = "Freeze")
	bool bIsFrozen = false;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UEnemyCombatComponent* EnemyCombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UCapsuleComponent* LeftHandCollisionCapsule;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FName LeftHandCollisionCapsuleAttachBoneName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UCapsuleComponent* RightHandCollisionCapsule;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FName RightHandCollisionCapsuleAttachBoneName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FName RightLegCollisionCapsuleAttachBoneName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UCapsuleComponent* RightLegCollisionCapsule;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FName LeftLegCollisionCapsuleAttachBoneName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UCapsuleComponent* LeftLegCollisionCapsule;

	UPROPERTY()
	TArray<UParticleSystemComponent*> ActiveFreezeVFX;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UEnemyUIComponent* EnemyUIComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* EnemyHealthWidgetComponent;


	virtual void PossessedBy(AController* NewController) override;
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	void Init_StartUpData();


public:
	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const { return EnemyCombatComponent; }

	FORCEINLINE UCapsuleComponent* GetLeftHandCollisionCapsule() const { return LeftHandCollisionCapsule; }

	// === FIX: Make sure this matches exactly - no typo! ===
	FORCEINLINE UCapsuleComponent* GetRightHandCollisionCapsule() const { return RightHandCollisionCapsule; }

	// === BOSS CHECK ===
	UFUNCTION(BlueprintPure, Category = "Boss")
	FORCEINLINE bool IsBoss() const { return bIsBoss; }
};