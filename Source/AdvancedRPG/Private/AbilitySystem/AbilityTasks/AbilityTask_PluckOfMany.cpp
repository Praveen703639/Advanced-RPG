#include "AbilitySystem/AbilityTasks/AbilityTask_PluckOfMany.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "TimerManager.h"

UAbilityTask_PluckOfMany* UAbilityTask_PluckOfMany::PluckOfMany(
	UGameplayAbility* OwningAbility,
	TSubclassOf<AWarriorEnemyCharacter> CloneClass,
	int32 CloneCount,
	float CloneLifetime,
	float SpawnRadius,
	float SpawnDelayPerClone,
	UParticleSystem* SpawnEffect,
	UParticleSystem* StrandPluckEffect,
	UParticleSystem* CloneDeathEffect)
{
	UAbilityTask_PluckOfMany* MyObj = NewAbilityTask<UAbilityTask_PluckOfMany>(OwningAbility);
	MyObj->CachedCloneClass = CloneClass;
	MyObj->CachedCloneCount = FMath::Max(CloneCount, 1);
	MyObj->CachedCloneLifetime = FMath::Max(CloneLifetime, 3.0f);
	MyObj->CachedSpawnRadius = FMath::Max(SpawnRadius, 100.0f);
	MyObj->CachedSpawnDelayPerClone = FMath::Max(SpawnDelayPerClone, 0.05f);
	MyObj->CachedSpawnEffect = SpawnEffect;
	MyObj->CachedStrandPluckEffect = StrandPluckEffect;
	MyObj->CachedCloneDeathEffect = CloneDeathEffect;
	return MyObj;
}

void UAbilityTask_PluckOfMany::Activate()
{
	Super::Activate();

	CurrentSpawnIndex = 0;
	TotalSpawnedCount = 0;
	bHasStartedSpawning = false;
	ActiveClones.Empty();
	CloneLifetimeTimers.Empty();

	if (!AbilitySystemComponent.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("PluckOfMany: ASC is invalid!"));
		if (ShouldBroadcastAbilityTaskDelegates()) { OnPluckFailed.Broadcast(); }
		EndTask();
		return;
	}

	SourceCharacter = Cast<ACharacter>(AbilitySystemComponent->GetAvatarActor());
	if (!SourceCharacter.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("PluckOfMany: AvatarActor is not a valid Character!"));
		if (ShouldBroadcastAbilityTaskDelegates()) { OnPluckFailed.Broadcast(); }
		EndTask();
		return;
	}

	if (!CachedCloneClass)
	{
		UE_LOG(LogTemp, Error, TEXT("PluckOfMany: CloneClass is null! Did you wire it in Blueprint?"));
		if (ShouldBroadcastAbilityTaskDelegates()) { OnPluckFailed.Broadcast(); }
		EndTask();
		return;
	}

	// >>> THIS IS THE FIX <<<
	// Previously you never actually started the spawn loop.
	StartSpawningClones();
}

void UAbilityTask_PluckOfMany::OnDestroy(bool bInOwnerFinished)
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(SpawnTimerHandle);
		for (auto& Pair : CloneLifetimeTimers)
		{
			World->GetTimerManager().ClearTimer(Pair.Value);
		}
	}

	for (TWeakObjectPtr<ACharacter> ClonePtr : ActiveClones)
	{
		if (ACharacter* Clone = ClonePtr.Get())
		{
			if (World && CachedCloneDeathEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(World, CachedCloneDeathEffect, Clone->GetActorLocation(), Clone->GetActorRotation());
			}
			if (IsValid(Clone))
			{
				Clone->Destroy();
			}
		}
	}

	ActiveClones.Empty();
	CloneLifetimeTimers.Empty();
	SourceCharacter.Reset();

	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_PluckOfMany::StartSpawningClones()
{
	if (!bHasStartedSpawning && SourceCharacter.IsValid() && CachedCloneClass)
	{
		bHasStartedSpawning = true;
		SpawnNextClone();
	}
}

FVector UAbilityTask_PluckOfMany::CalculateSpawnLocation(int32 CloneIndex) const
{
	if (!SourceCharacter.IsValid())
	{
		return FVector::ZeroVector;
	}

	FVector Center = SourceCharacter->GetActorLocation();
	FRotator Rotation = SourceCharacter->GetActorRotation();

	float ArcAngle = 120.0f;
	float AngleOffset = -ArcAngle / 2.0f + (ArcAngle * CloneIndex) / FMath::Max(CachedCloneCount - 1, 1);
	FRotator ArcRot = Rotation + FRotator(0.0f, AngleOffset, 0.0f);
	FVector SpawnLoc = Center + ArcRot.Vector() * CachedSpawnRadius;

	if (UWorld* World = GetWorld())
	{
		if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World))
		{
			FNavLocation NavLoc;
			if (NavSys->ProjectPointToNavigation(SpawnLoc, NavLoc, FVector(500.0f, 500.0f, 500.0f)))
			{
				SpawnLoc = NavLoc.Location;
			}
		}
	}

	return SpawnLoc;
}

void UAbilityTask_PluckOfMany::SpawnNextClone()
{
	if (!SourceCharacter.IsValid())
	{
		EndTask();
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		EndTask();
		return;
	}

	World->GetTimerManager().ClearTimer(SpawnTimerHandle);

	if (CurrentSpawnIndex >= CachedCloneCount)
	{
		FinishCloneSpawning();
		return;
	}

	FVector SpawnLoc = CalculateSpawnLocation(CurrentSpawnIndex);
	FRotator SpawnRot = (SourceCharacter->GetActorLocation() - SpawnLoc).Rotation();

	// Strand trail from source to spawn point
	if (CachedStrandPluckEffect && SourceCharacter.IsValid())
	{
		FVector StartLoc = SourceCharacter->GetActorLocation() + FVector(0.0f, 0.0f, 100.0f);
		FVector EndLoc = SpawnLoc;
		FVector MidPoint = (StartLoc + EndLoc) * 0.5f + FVector(0.0f, 0.0f, 150.0f);

		const int32 TrailSegments = 8;
		for (int32 j = 0; j <= TrailSegments; ++j)
		{
			float T = static_cast<float>(j) / TrailSegments;
			FVector TrailLoc = FMath::Lerp(FMath::Lerp(StartLoc, MidPoint, T), FMath::Lerp(MidPoint, EndLoc, T), T);
			UGameplayStatics::SpawnEmitterAtLocation(World, CachedStrandPluckEffect, TrailLoc, (EndLoc - StartLoc).Rotation(), true);
		}
	}

	// Ground spawn puff
	if (CachedSpawnEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(World, CachedSpawnEffect, SpawnLoc, SpawnRot, true);
	}

	// Spawn clone
	FActorSpawnParameters SpawnParam;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParam.Instigator = Cast<APawn>(SourceCharacter.Get());
	SpawnParam.Owner = SourceCharacter.Get();

	ACharacter* NewClone = World->SpawnActor<ACharacter>(CachedCloneClass, SpawnLoc, SpawnRot, SpawnParam);

	if (!NewClone)
	{
		UE_LOG(LogTemp, Error, TEXT("PluckOfMany: SpawnActor FAILED for clone %d/%d at %s"), CurrentSpawnIndex + 1, CachedCloneCount, *SpawnLoc.ToString());

		++CurrentSpawnIndex;
		if (CurrentSpawnIndex < CachedCloneCount)
		{
			World->GetTimerManager().SetTimer(SpawnTimerHandle, this, &UAbilityTask_PluckOfMany::SpawnNextClone, CachedSpawnDelayPerClone, false);
		}
		else
		{
			FinishCloneSpawning();
		}
		return;
	}

	ActiveClones.Add(NewClone);
	++TotalSpawnedCount;

	UE_LOG(LogTemp, Log, TEXT("PluckOfMany: Spawned clone %d/%d — %s"), TotalSpawnedCount, CachedCloneCount, *GetNameSafe(NewClone));

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCloneSpawned.Broadcast(NewClone);
	}

	// Lifetime timer
	FTimerHandle& CloneTimer = CloneLifetimeTimers.Add(NewClone);
	World->GetTimerManager().SetTimer(CloneTimer, FTimerDelegate::CreateUObject(this, &ThisClass::OnCloneLifetimeExpired, NewClone), CachedCloneLifetime, false);

	++CurrentSpawnIndex;
	if (CurrentSpawnIndex < CachedCloneCount)
	{
		World->GetTimerManager().SetTimer(SpawnTimerHandle, this, &UAbilityTask_PluckOfMany::SpawnNextClone, CachedSpawnDelayPerClone, false);
	}
	else
	{
		FinishCloneSpawning();
	}
}

void UAbilityTask_PluckOfMany::FinishCloneSpawning()
{
	if (ActiveClones.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("PluckOfMany: Zero clones alive!"));
		if (ShouldBroadcastAbilityTaskDelegates()) { OnPluckFailed.Broadcast(); }
		EndTask();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("PluckOfMany: Finished spawning %d clones."), ActiveClones.Num());
	}
}

void UAbilityTask_PluckOfMany::OnCloneLifetimeExpired(ACharacter* Clone)
{
	if (!Clone || !IsValid(Clone))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World && CachedCloneDeathEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(World, CachedCloneDeathEffect, Clone->GetActorLocation(), Clone->GetActorRotation());
	}

	Clone->Destroy();

	ActiveClones.Remove(Clone);
	CloneLifetimeTimers.Remove(Clone);

	if (ActiveClones.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("PluckOfMany: All clones expired. Total spawned: %d"), TotalSpawnedCount);
		if (ShouldBroadcastAbilityTaskDelegates()) { OnAllClonesExpired.Broadcast(TotalSpawnedCount); }
		EndTask();
	}
}

void UAbilityTask_PluckOfMany::ExpireAllClones()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(SpawnTimerHandle);
	}

	TArray<TWeakObjectPtr<ACharacter>> ClonesToExpire = ActiveClones;
	for (TWeakObjectPtr<ACharacter> ClonePtr : ClonesToExpire)
	{
		if (ACharacter* Clone = ClonePtr.Get())
		{
			if (FTimerHandle* Timer = CloneLifetimeTimers.Find(Clone))
			{
				if (World)
				{
					World->GetTimerManager().ClearTimer(*Timer);
				}
			}

			if (World && CachedCloneDeathEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(World, CachedCloneDeathEffect, Clone->GetActorLocation(), Clone->GetActorRotation());
			}

			if (IsValid(Clone))
			{
				Clone->Destroy();
			}
		}
	}

	ActiveClones.Empty();
	CloneLifetimeTimers.Empty();

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnAllClonesExpired.Broadcast(TotalSpawnedCount);
	}
	EndTask();
}