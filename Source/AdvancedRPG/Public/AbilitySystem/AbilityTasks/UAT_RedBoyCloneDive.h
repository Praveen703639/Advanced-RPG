// pravin's patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "UAT_RedBoyCloneDive.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRedBoyCloneDiveComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCloneHitTarget, AActor*, InHitTargetActor);

USTRUCT()
struct FCloneDiveData
{
	GENERATED_BODY()
	TWeakObjectPtr<AActor> CloneActor;
	FVector StartLocation;
	FVector TargetLocation;
	float StartTime;
	float DiveDuration;
	bool bHasImpacted;
};

USTRUCT()
struct FCloneOrbitData
{
	GENERATED_BODY()
	TWeakObjectPtr<AActor> CloneActor;
	float BaseAngle;
	float CurrentAngle;
	bool bHasDived;
};

UCLASS()
class ADVANCEDRPG_API UUAT_RedBoyCloneDive : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnRedBoyCloneDiveComplete OnCloneDiveComplete;

	UPROPERTY(BlueprintAssignable)
	FOnCloneHitTarget OnCloneHitTarget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Red Boy Clone Dive")
	UAnimMontage* FlyInAirMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Red Boy Clone Dive")
	UAnimMontage* DodgeMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Red Boy Clone Dive")
	UParticleSystem* CloneSpawnCircleFX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Red Boy Clone Dive")
	UParticleSystem* CloneDiveTrailFX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Red Boy Clone Dive")
	UParticleSystem* HitFX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Red Boy Clone Dive")
	UParticleSystem* BossLandingFX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Red Boy Clone Dive")
	USoundBase* CircleSpawnSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Red Boy Clone Dive")
	USoundBase* DodgeSoundFX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Red Boy Clone Dive")
	USoundBase* HitSoundFX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Red Boy Clone Dive")
	USoundBase* BossLandingSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Red Boy Clone Dive")
	TSubclassOf<AActor> CloneCharacterClass;

	UPROPERTY(BlueprintReadWrite, Category = "Red Boy Clone Dive")
	int32 Wave1Count = 1;

	UPROPERTY(BlueprintReadWrite, Category = "Red Boy Clone Dive")
	int32 Wave2Count = 2;

	UPROPERTY(BlueprintReadWrite, Category = "Red Boy Clone Dive")
	int32 Wave3Count = 3;

	UPROPERTY(BlueprintReadWrite, Category = "Red Boy Clone Dive")
	float RiseHeight = 900.f;

	UPROPERTY(BlueprintReadWrite, Category = "Red Boy Clone Dive")
	float RiseDuration = 1.5f;

	UPROPERTY(BlueprintReadWrite, Category = "Red Boy Clone Dive")
	float OrbitDuration = 5.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Red Boy Clone Dive")
	float OrbitRadius = 400.f;

	UPROPERTY(BlueprintReadWrite, Category = "Red Boy Clone Dive")
	float OrbitSpeed = 60.f;

	UPROPERTY(BlueprintReadWrite, Category = "Red Boy Clone Dive")
	float OrbitHeightOffset = 100.f;

	UPROPERTY(BlueprintReadWrite, Category = "Red Boy Clone Dive")
	float CloneDiveSpeed = 2200.f;

	UPROPERTY(BlueprintReadWrite, Category = "Red Boy Clone Dive")
	float TimeBetweenWaves = 1.2f;

	UPROPERTY(BlueprintReadWrite, Category = "Red Boy Clone Dive")
	float CloneDamage = 50.f;

	UPROPERTY(BlueprintReadWrite, Category = "Red Boy Clone Dive")
	float CloneDamageRadius = 280.f;

	UPROPERTY(BlueprintReadWrite, Category = "Red Boy Clone Dive")
	float BossLandingDamage = 125.f;

	UPROPERTY(BlueprintReadWrite, Category = "Red Boy Clone Dive")
	float BossLandingRadius = 400.f;

	UPROPERTY(BlueprintReadWrite, Category = "Red Boy Clone Dive")
	float BossFallDuration = 0.8f;

	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UUAT_RedBoyCloneDive* RedBoyCloneDive(
		UGameplayAbility* OwningAbility,
		UAnimMontage* InFlyInAirMontage,
		UAnimMontage* InDodgeMontage,
		UParticleSystem* InCloneSpawnCircleFX,
		UParticleSystem* InCloneDiveTrailFX,
		UParticleSystem* InHitFX,
		UParticleSystem* InBossLandingFX,
		USoundBase* InCircleSpawnSound,
		USoundBase* InDodgeSoundFX,
		USoundBase* InHitSoundFX,
		USoundBase* InBossLandingSound,
		TSubclassOf<AActor> InCloneCharacterClass);

	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual void OnDestroy(bool AbilityEnded) override;

protected:
	UPROPERTY()
	TArray<AActor*> SpawnedClones;

	UPROPERTY()
	TArray<UParticleSystemComponent*> SpawnedParticles;

	TArray<FCloneDiveData> ActiveClones;
	TArray<FCloneOrbitData> OrbitingClones;

	FVector OriginalBossLocation;
	FVector AirborneLocation;
	FVector TargetLandingLocation;
	TWeakObjectPtr<AActor> TargetActor;

	FTimerHandle RiseTimer;
	FTimerHandle FallTimer;
	FTimerHandle SequenceTimer;
	FTimerHandle MoveClonesTimer;
	FTimerHandle OrbitTimerHandle;

	int32 CurrentWave;
	bool bIsRising;
	bool bIsFalling;
	bool bIsOrbitActive;
	float RiseStartTime;
	float FallStartTime;

	void StartRise();
	void UpdateRise();
	void OnRiseComplete();

	void SpawnAllClonesForOrbit();
	void UpdateCloneOrbit();
	void OnOrbitComplete();

	void ReleaseDiveWave(int32 WaveIndex);
	void OnDiveWaveComplete();
	void UpdateClonePositions();
	void OnCloneImpact(AActor* CloneActor, const FVector& ImpactLocation);

	void StartBossFall();
	void UpdateBossFall();
	void OnBossLanded();

	void CleanupAndEnd();

	FVector GetPlayerGroundLocation() const;
	void PlayMontageOnActor(AActor* Actor, UAnimMontage* Montage);

	// === VRAM FIX ===
	UPROPERTY()
	int32 TargetCloneCount = 0;

	int32 SpawnedCloneIndex = 0;
	bool bClonesSpawned = false;
	FTimerHandle StaggerSpawnTimer;

	void StaggerSpawnNextClone();
	void FinishCloneSpawnSequence();
};