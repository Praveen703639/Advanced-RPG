// pravin's patashala all Rights Reserved 

#include "AbilitySystem/AbilityTasks/UAT_RedBoyCloneDive.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundBase.h"

UUAT_RedBoyCloneDive* UUAT_RedBoyCloneDive::RedBoyCloneDive(
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
	TSubclassOf<AActor> InCloneCharacterClass)
{
	UUAT_RedBoyCloneDive* Task = NewAbilityTask<UUAT_RedBoyCloneDive>(OwningAbility);
	Task->FlyInAirMontage = InFlyInAirMontage;
	Task->DodgeMontage = InDodgeMontage;
	Task->CloneSpawnCircleFX = InCloneSpawnCircleFX;
	Task->CloneDiveTrailFX = InCloneDiveTrailFX;
	Task->HitFX = InHitFX;
	Task->BossLandingFX = InBossLandingFX;
	Task->CircleSpawnSound = InCircleSpawnSound;
	Task->DodgeSoundFX = InDodgeSoundFX;
	Task->HitSoundFX = InHitSoundFX;
	Task->BossLandingSound = InBossLandingSound;
	Task->CloneCharacterClass = InCloneCharacterClass;
	return Task;
}

void UUAT_RedBoyCloneDive::Activate()
{
	if (!Ability || !Ability->GetCurrentActorInfo()->AvatarActor.IsValid())
	{
		EndTask();
		return;
	}

	AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	OriginalBossLocation = Avatar->GetActorLocation();

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player)
	{
		EndTask();
		return;
	}
	TargetActor = Player;

	bIsOrbitActive = false;
	bIsRising = false;
	bIsFalling = false;
	CurrentWave = 0;

	StartRise();
}

void UUAT_RedBoyCloneDive::ExternalCancel()
{
	CleanupAndEnd();
	Super::ExternalCancel();
}

void UUAT_RedBoyCloneDive::OnDestroy(bool AbilityEnded)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}
	Super::OnDestroy(AbilityEnded);
}

void UUAT_RedBoyCloneDive::StartRise()
{
	AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (!Avatar) { CleanupAndEnd(); return; }

	PlayMontageOnActor(Avatar, FlyInAirMontage);

	if (ACharacter* BossChar = Cast<ACharacter>(Avatar))
	{
		if (UCharacterMovementComponent* MoveComp = BossChar->GetCharacterMovement())
		{
			MoveComp->DisableMovement();
			MoveComp->StopMovementImmediately();
		}
	}

	bIsRising = true;
	RiseStartTime = GetWorld()->GetTimeSeconds();
	GetWorld()->GetTimerManager().SetTimer(RiseTimer, this, &UUAT_RedBoyCloneDive::UpdateRise, 0.02f, true);
}

void UUAT_RedBoyCloneDive::UpdateRise()
{
	AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (!Avatar) { CleanupAndEnd(); return; }

	float Elapsed = GetWorld()->GetTimeSeconds() - RiseStartTime;
	float Alpha = FMath::Clamp(Elapsed / RiseDuration, 0.f, 1.f);
	Alpha = Alpha * Alpha * (3.0f - 2.0f * Alpha); // smoothstep

	FVector TargetLoc = OriginalBossLocation + FVector(0.f, 0.f, RiseHeight);
	FVector NewLoc = FMath::Lerp(OriginalBossLocation, TargetLoc, Alpha);
	Avatar->SetActorLocation(NewLoc, false, nullptr, ETeleportType::TeleportPhysics);

	if (Alpha >= 1.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(RiseTimer);
		OnRiseComplete();
	}
}

void UUAT_RedBoyCloneDive::OnRiseComplete()
{
	AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (Avatar)
	{
		if (ACharacter* BossChar = Cast<ACharacter>(Avatar))
		{
			if (USkeletalMeshComponent* Mesh = BossChar->GetMesh())
			{
				Mesh->SetVisibility(false, true);
			}
		}
		AirborneLocation = Avatar->GetActorLocation();
	}

	SpawnAllClonesForOrbit();
}

void UUAT_RedBoyCloneDive::SpawnAllClonesForOrbit()
{
	// === GUARD: Never run this twice ===
	if (bClonesSpawned || !GetWorld())
	{
		OnOrbitComplete();
		return;
	}
	bClonesSpawned = true;

	if (!CloneCharacterClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("RedBoyCloneDive: CloneCharacterClass is NULL!"));
		OnOrbitComplete();
		return;
	}

	// === HARD CAP: Absolute maximum clones to prevent VRAM death ===
	TargetCloneCount = Wave1Count + Wave2Count + Wave3Count;
	if (TargetCloneCount > 8)
	{
		UE_LOG(LogTemp, Error, TEXT("RedBoyCloneDive: Clone count %d exceeds VRAM safety limit of 8! Capping to 8."), TargetCloneCount);
		TargetCloneCount = 8;
	}
	if (TargetCloneCount <= 0)
	{
		OnOrbitComplete();
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("RedBoyCloneDive: Starting staggered spawn of %d clones."), TargetCloneCount);

	AActor* Boss = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (!Boss) { CleanupAndEnd(); return; }

	const FVector Center = AirborneLocation;

	// FX & Sound at center (only once)
	if (CloneSpawnCircleFX)
	{
		if (UParticleSystemComponent* CirclePSC = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), CloneSpawnCircleFX, Center, FRotator::ZeroRotator, true))
		{
			SpawnedParticles.Add(CirclePSC);
		}
	}

	if (CircleSpawnSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CircleSpawnSound, Center);
	}

	// === STAGGERED SPAWN: 1 clone every 0.05s to prevent VRAM spike ===
	SpawnedCloneIndex = 0;
	GetWorld()->GetTimerManager().SetTimer(StaggerSpawnTimer, this, &UUAT_RedBoyCloneDive::StaggerSpawnNextClone, 0.05f, true);
}

void UUAT_RedBoyCloneDive::UpdateCloneOrbit()
{
	if (!bIsOrbitActive) return;

	const FVector Center = AirborneLocation; // FIXED: don't query boss location
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	const float DeltaAngle = OrbitSpeed * DeltaTime;

	for (FCloneOrbitData& Data : OrbitingClones)
	{
		if (!Data.CloneActor.IsValid() || Data.bHasDived) continue;

		Data.CurrentAngle += DeltaAngle;
		float Rad = FMath::DegreesToRadians(Data.CurrentAngle);

		FVector NewLoc = Center + FVector(
			FMath::Cos(Rad) * OrbitRadius,
			FMath::Sin(Rad) * OrbitRadius,
			OrbitHeightOffset
		);

		// TeleportPhysics = zero collision sweep cost
		Data.CloneActor->SetActorLocation(NewLoc, false, nullptr, ETeleportType::TeleportPhysics);

		// Cheaper rotation: vector to center is just negative of offset
		FRotator LookAtCenter = (Center - NewLoc).Rotation();
		Data.CloneActor->SetActorRotation(LookAtCenter);
	}
}

void UUAT_RedBoyCloneDive::OnOrbitComplete()
{
	bIsOrbitActive = false;
	if (GetWorld()) GetWorld()->GetTimerManager().ClearTimer(OrbitTimerHandle);

	CurrentWave = 0;
	ReleaseDiveWave(0);
}

void UUAT_RedBoyCloneDive::ReleaseDiveWave(int32 WaveIndex)
{
	if (!GetWorld()) return;

	CurrentWave = WaveIndex + 1;

	int32 StartIndex = 0;
	int32 Count = 0;

	if (CurrentWave == 1) { StartIndex = 0; Count = Wave1Count; }
	else if (CurrentWave == 2) { StartIndex = Wave1Count; Count = Wave2Count; }
	else if (CurrentWave == 3) { StartIndex = Wave1Count + Wave2Count; Count = Wave3Count; }
	else { StartBossFall(); return; }

	FVector PlayerLoc = GetPlayerGroundLocation();

	for (int32 i = StartIndex; i < StartIndex + Count && i < OrbitingClones.Num(); ++i)
	{
		FCloneOrbitData& OrbitData = OrbitingClones[i];
		if (!OrbitData.CloneActor.IsValid()) continue;

		OrbitData.bHasDived = true;
		AActor* Clone = OrbitData.CloneActor.Get();

		// Face the dive target immediately
		FVector StartLoc = Clone->GetActorLocation();
		float AngleRad = FMath::DegreesToRadians(OrbitData.CurrentAngle);
		FVector TargetLoc = PlayerLoc + FVector(
			FMath::Cos(AngleRad) * 150.f,
			FMath::Sin(AngleRad) * 150.f,
			0.f // PlayerLoc already has correct ground Z
		);

		FRotator DiveRot = (TargetLoc - StartLoc).Rotation();
		Clone->SetActorRotation(DiveRot);

		// Play montage (MUST NOT have root motion! Use in-place dodge anim)
		PlayMontageOnActor(Clone, DodgeMontage);

		if (DodgeSoundFX)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DodgeSoundFX, StartLoc);
		}

		// Trail FX attached to mesh
		if (CloneDiveTrailFX)
		{
			if (ACharacter* CloneChar = Cast<ACharacter>(Clone))
			{
				if (USkeletalMeshComponent* SkelMesh = CloneChar->GetMesh())
				{
					UParticleSystemComponent* TrailPSC = UGameplayStatics::SpawnEmitterAttached(
						CloneDiveTrailFX, SkelMesh, NAME_None,
						FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
					if (TrailPSC) SpawnedParticles.Add(TrailPSC);
				}
			}
		}

		float Dist = FVector::Dist(StartLoc, TargetLoc);
		float DiveTime = Dist / CloneDiveSpeed;

		FCloneDiveData DiveData;
		DiveData.CloneActor = Clone;
		DiveData.StartLocation = StartLoc;
		DiveData.TargetLocation = TargetLoc;
		DiveData.StartTime = GetWorld()->GetTimeSeconds();
		DiveData.DiveDuration = FMath::Max(DiveTime, 0.1f);
		DiveData.bHasImpacted = false;
		ActiveClones.Add(DiveData);
	}

	if (!MoveClonesTimer.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(MoveClonesTimer, this, &UUAT_RedBoyCloneDive::UpdateClonePositions, 0.02f, true);
	}

	GetWorld()->GetTimerManager().SetTimer(SequenceTimer, this, &UUAT_RedBoyCloneDive::OnDiveWaveComplete, TimeBetweenWaves, false);
}

void UUAT_RedBoyCloneDive::OnDiveWaveComplete()
{
	ReleaseDiveWave(CurrentWave);
}

void UUAT_RedBoyCloneDive::UpdateClonePositions()
{
	if (!GetWorld()) return;

	float Now = GetWorld()->GetTimeSeconds();
	bool bAnyActive = false;

	// Reverse iterate so we can remove finished entries safely
	for (int32 i = ActiveClones.Num() - 1; i >= 0; --i)
	{
		FCloneDiveData& Data = ActiveClones[i];

		if (!Data.CloneActor.IsValid())
		{
			ActiveClones.RemoveAtSwap(i, 1, EAllowShrinking::No);
			continue;
		}

		if (Data.bHasImpacted)
		{
			ActiveClones.RemoveAtSwap(i, 1, EAllowShrinking::No);
			continue;
		}

		float Alpha = (Now - Data.StartTime) / Data.DiveDuration;

		if (Alpha >= 1.f)
		{
			Data.CloneActor->SetActorLocation(Data.TargetLocation, false, nullptr, ETeleportType::TeleportPhysics);
			Data.bHasImpacted = true;
			OnCloneImpact(Data.CloneActor.Get(), Data.TargetLocation);
			ActiveClones.RemoveAtSwap(i, 1, EAllowShrinking::No);
		}
		else
		{
			float EasedAlpha = FMath::InterpEaseIn(0.f, 1.f, Alpha, 2.f);
			FVector NewLoc = FMath::Lerp(Data.StartLocation, Data.TargetLocation, EasedAlpha);
			Data.CloneActor->SetActorLocation(NewLoc, false, nullptr, ETeleportType::TeleportPhysics);
			bAnyActive = true;
		}
	}

	if (!bAnyActive && ActiveClones.Num() == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(MoveClonesTimer);
	}
}

void UUAT_RedBoyCloneDive::OnCloneImpact(AActor* CloneActor, const FVector& ImpactLocation)
{
	if (!CloneActor || !GetWorld()) return;

	if (HitFX)
	{
		if (UParticleSystemComponent* HitPSC = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), HitFX, ImpactLocation, FRotator::ZeroRotator, true))
		{
			SpawnedParticles.Add(HitPSC);
		}
	}

	if (HitSoundFX)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSoundFX, ImpactLocation);
	}

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(CloneActor);
	if (Ability && Ability->GetCurrentActorInfo()->AvatarActor.IsValid())
	{
		IgnoreActors.Add(Ability->GetCurrentActorInfo()->AvatarActor.Get());
	}

	UGameplayStatics::ApplyRadialDamage(
		GetWorld(), CloneDamage, ImpactLocation, CloneDamageRadius,
		nullptr, IgnoreActors, CloneActor, nullptr, true);

	if (TargetActor.IsValid())
	{
		AActor* Hero = TargetActor.Get();
		float DistToHero = FVector::Dist(ImpactLocation, Hero->GetActorLocation());
		if (DistToHero <= CloneDamageRadius)
		{
			OnCloneHitTarget.Broadcast(Hero);
		}
	}

	// Hide immediately, destroy shortly after (trail FX can finish)
	CloneActor->SetActorHiddenInGame(true);
	CloneActor->SetLifeSpan(0.25f);
}

void UUAT_RedBoyCloneDive::StartBossFall()
{
	AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (!Avatar) { CleanupAndEnd(); return; }

	if (ACharacter* BossChar = Cast<ACharacter>(Avatar))
	{
		if (USkeletalMeshComponent* Mesh = BossChar->GetMesh())
		{
			Mesh->SetVisibility(true, true);
		}
	}

	FVector PlayerLoc = GetPlayerGroundLocation();
	TargetLandingLocation = PlayerLoc;
	TargetLandingLocation.Z = OriginalBossLocation.Z; // Boss lands at his original height level

	FVector FallStart = FVector(PlayerLoc.X, PlayerLoc.Y, AirborneLocation.Z);
	Avatar->SetActorLocation(FallStart, false, nullptr, ETeleportType::TeleportPhysics);

	bIsFalling = true;
	FallStartTime = GetWorld()->GetTimeSeconds();
	GetWorld()->GetTimerManager().SetTimer(FallTimer, this, &UUAT_RedBoyCloneDive::UpdateBossFall, 0.02f, true);
}

void UUAT_RedBoyCloneDive::UpdateBossFall()
{
	AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (!Avatar) { CleanupAndEnd(); return; }

	float Elapsed = GetWorld()->GetTimeSeconds() - FallStartTime;
	float Alpha = FMath::Clamp(Elapsed / BossFallDuration, 0.f, 1.f);
	Alpha = FMath::InterpEaseIn(0.f, 1.f, Alpha, 3.f);

	FVector FallStart = FVector(TargetLandingLocation.X, TargetLandingLocation.Y, AirborneLocation.Z);
	FVector NewLoc = FMath::Lerp(FallStart, TargetLandingLocation, Alpha);
	Avatar->SetActorLocation(NewLoc, false, nullptr, ETeleportType::TeleportPhysics);

	if (Alpha >= 1.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(FallTimer);
		OnBossLanded();
	}
}

void UUAT_RedBoyCloneDive::OnBossLanded()
{
	AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (!Avatar) { CleanupAndEnd(); return; }

	if (BossLandingFX)
	{
		if (UParticleSystemComponent* LandPSC = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), BossLandingFX, TargetLandingLocation, FRotator::ZeroRotator, true))
		{
			SpawnedParticles.Add(LandPSC);
		}
	}

	if (BossLandingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BossLandingSound, TargetLandingLocation);
	}

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Avatar);
	UGameplayStatics::ApplyRadialDamage(
		GetWorld(), BossLandingDamage, TargetLandingLocation, BossLandingRadius,
		nullptr, IgnoreActors, Avatar, nullptr, true);

	if (ACharacter* BossChar = Cast<ACharacter>(Avatar))
	{
		if (UCharacterMovementComponent* MoveComp = BossChar->GetCharacterMovement())
		{
			MoveComp->SetMovementMode(MOVE_Walking);
		}
	}

	GetWorld()->GetTimerManager().SetTimer(SequenceTimer, this, &UUAT_RedBoyCloneDive::CleanupAndEnd, 1.0f, false);
}

void UUAT_RedBoyCloneDive::CleanupAndEnd()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RiseTimer);
		World->GetTimerManager().ClearTimer(FallTimer);
		World->GetTimerManager().ClearTimer(SequenceTimer);
		World->GetTimerManager().ClearTimer(MoveClonesTimer);
		World->GetTimerManager().ClearTimer(OrbitTimerHandle);
		World->GetTimerManager().ClearTimer(StaggerSpawnTimer);
	}

	for (AActor* Clone : SpawnedClones)
	{
		if (IsValid(Clone))
		{
			Clone->SetActorHiddenInGame(false);
			Clone->Destroy();
		}
	}
	SpawnedClones.Empty();

	for (UParticleSystemComponent* PSC : SpawnedParticles)
	{
		if (IsValid(PSC))
		{
			PSC->DeactivateSystem();
			PSC->DestroyComponent();
		}
	}
	SpawnedParticles.Empty();

	ActiveClones.Empty();
	OrbitingClones.Empty();

	if (Ability && Ability->GetCurrentActorInfo()->AvatarActor.IsValid())
	{
		AActor* Avatar = Ability->GetCurrentActorInfo()->AvatarActor.Get();
		if (ACharacter* BossChar = Cast<ACharacter>(Avatar))
		{
			if (USkeletalMeshComponent* Mesh = BossChar->GetMesh())
			{
				Mesh->SetVisibility(true, true);
			}
			if (UCharacterMovementComponent* MoveComp = BossChar->GetCharacterMovement())
			{
				MoveComp->SetMovementMode(MOVE_Walking);
			}
		}
	}

	OnCloneDiveComplete.Broadcast();
	EndTask();
}

FVector UUAT_RedBoyCloneDive::GetPlayerGroundLocation() const
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player) return FVector::ZeroVector;

	FVector PlayerLoc = Player->GetActorLocation();

	// FIXED: Trace down to actual ground instead of assuming boss's old Z
	FVector TraceStart = PlayerLoc + FVector(0.f, 0.f, 500.f);
	FVector TraceEnd = PlayerLoc - FVector(0.f, 0.f, 5000.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Player);

	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params))
	{
		return Hit.Location;
	}

	// Fallback: use player's current Z if trace fails
	return PlayerLoc;
}

void UUAT_RedBoyCloneDive::PlayMontageOnActor(AActor* Actor, UAnimMontage* Montage)
{
	if (!Actor || !Montage) return;
	ACharacter* Char = Cast<ACharacter>(Actor);
	if (Char && Char->GetMesh() && Char->GetMesh()->GetAnimInstance())
	{
		Char->GetMesh()->GetAnimInstance()->Montage_Play(Montage);
	}
}

void UUAT_RedBoyCloneDive::StaggerSpawnNextClone()
{
	if (!GetWorld()) return;

	AActor* Boss = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (!Boss) { CleanupAndEnd(); return; }

	const FVector Center = AirborneLocation;
	const int32 i = SpawnedCloneIndex;

	// Calculate angle for this clone
	float AngleDeg = (360.f / TargetCloneCount) * i;
	float AngleRad = FMath::DegreesToRadians(AngleDeg);

	FVector SpawnLoc = Center + FVector(
		FMath::Cos(AngleRad) * OrbitRadius,
		FMath::Sin(AngleRad) * OrbitRadius,
		OrbitHeightOffset
	);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = Boss;

	AActor* Clone = GetWorld()->SpawnActor<AActor>(CloneCharacterClass, SpawnLoc, FRotator::ZeroRotator, Params);

	if (Clone)
	{
		SpawnedClones.Add(Clone);

		// === VRAM OPTIMIZATIONS: Kill all expensive rendering features ===
		if (ACharacter* CloneChar = Cast<ACharacter>(Clone))
		{
			// Disable movement & physics
			if (UCharacterMovementComponent* MoveComp = CloneChar->GetCharacterMovement())
			{
				MoveComp->DisableMovement();
				MoveComp->StopMovementImmediately();
				MoveComp->GravityScale = 0.f;
			}
			CloneChar->SetActorTickEnabled(false);

			// Disable ALL collision
			CloneChar->SetActorEnableCollision(false);

			// Disable expensive rendering on skeletal mesh
			if (USkeletalMeshComponent* Skel = CloneChar->GetMesh())
			{
				Skel->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				Skel->SetGenerateOverlapEvents(false);

				// === THESE LINES SAVE VRAM ===
				Skel->bCastDynamicShadow = false;           // No dynamic shadows
				Skel->bCastStaticShadow = false;            // No static shadows
				Skel->bAffectDynamicIndirectLighting = false;
				Skel->bAffectDistanceFieldLighting = false;
				Skel->bRenderCustomDepth = false;
				Skel->SetVisibility(true);                  // Keep visible, but cheap
			}
		}
		else
		{
			// Non-character clone: still disable what we can
			Clone->SetActorTickEnabled(false);
			Clone->SetActorEnableCollision(false);

			if (USkeletalMeshComponent* Skel = Clone->FindComponentByClass<USkeletalMeshComponent>())
			{
				Skel->bCastDynamicShadow = false;
				Skel->bCastStaticShadow = false;
				Skel->bAffectDynamicIndirectLighting = false;
				Skel->bAffectDistanceFieldLighting = false;
			}
		}

		// Face center
		FRotator LookAtCenter = (Center - SpawnLoc).Rotation();
		Clone->SetActorRotation(LookAtCenter);

		// Store orbit data
		FCloneOrbitData Data;
		Data.CloneActor = Clone;
		Data.BaseAngle = AngleDeg;
		Data.CurrentAngle = AngleDeg;
		Data.bHasDived = false;
		OrbitingClones.Add(Data);

		UE_LOG(LogTemp, Log, TEXT("RedBoyCloneDive: Spawned clone %d/%d"), i + 1, TargetCloneCount);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RedBoyCloneDive: Failed to spawn clone %d!"), i);
	}

	SpawnedCloneIndex++;

	// Done spawning?
	if (SpawnedCloneIndex >= TargetCloneCount)
	{
		GetWorld()->GetTimerManager().ClearTimer(StaggerSpawnTimer);
		FinishCloneSpawnSequence();
	}
}

void UUAT_RedBoyCloneDive::FinishCloneSpawnSequence()
{
	UE_LOG(LogTemp, Log, TEXT("RedBoyCloneDive: All clones spawned. Starting orbit."));

	bIsOrbitActive = true;
	GetWorld()->GetTimerManager().SetTimer(OrbitTimerHandle, this, &UUAT_RedBoyCloneDive::UpdateCloneOrbit, 0.02f, true);
	GetWorld()->GetTimerManager().SetTimer(SequenceTimer, this, &UUAT_RedBoyCloneDive::OnOrbitComplete, OrbitDuration, false);
}


