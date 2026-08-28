// pravin's  patashala all Rights Reserved 

#include "WarriorSurvivalGameMode.h"
#include "Engine/AssetManager.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"
#include "NavigationSystem.h"
#include "WarriorFunctionLibrary.h"

#include "warriorDebugHelper.h"

void AWarriorSurvivalGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	EWarriorGameDifficulty SavedGameDifficulty;

	if (UWarriorFunctionLibrary::TryLoadSavedGameDifficulty(SavedGameDifficulty))
	{
		CurrentGameDifficulty = SavedGameDifficulty;
	}

}

void AWarriorSurvivalGameMode::SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState InState)
{
	CurrentSurvialGameModeState = InState;

	OnSurvialGameModeStateChanged.Broadcast(CurrentSurvialGameModeState);
}

void AWarriorSurvivalGameMode::BeginPlay()
{
	Super::BeginPlay();

	checkf(EnemyWaveSpawnerDataTable, TEXT("Forgot to assign a valid data table in survival game mode blueprint"));

	SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState::WaitSpawnNewWave);

	TotalWavesToSpawn = EnemyWaveSpawnerDataTable->GetRowNames().Num();

	PreLoadNextWaveEnemies();
}

void AWarriorSurvivalGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentSurvialGameModeState == EWarriorSurvialGameModeState::WaitSpawnNewWave)
	{
		TimePassedSinceStart += DeltaSeconds;
		if (TimePassedSinceStart >= SpawnNewWaveWaitTime)
		{
			TimePassedSinceStart = 0.f;

			SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState::SpawningNewWave);
		}
	}

	if (CurrentSurvialGameModeState == EWarriorSurvialGameModeState::SpawningNewWave)
	{
		TimePassedSinceStart += DeltaSeconds;

		if (TimePassedSinceStart >= SpawnEnemiesDelayTime)
		{
			//->   Handle spawn new enemies
			CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();

			TimePassedSinceStart = 0.f;

			SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState::InProgress);
		}
	}

	if (CurrentSurvialGameModeState == EWarriorSurvialGameModeState::WaveCompleted)
	{
		TimePassedSinceStart += DeltaSeconds;

		if (TimePassedSinceStart >= WaveCompletedWaitTime)
		{
			TimePassedSinceStart = 0.f;

			CurrentWaveCount++; // Advances from 0 to 1, 1 to 2, etc.

			if (HasFinishedAllWaves())
			{
				SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState::AllWavesDone);
			}
			else
			{
				SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState::WaitSpawnNewWave);
				PreLoadNextWaveEnemies();
			}
		}
	}
}

bool AWarriorSurvivalGameMode::HasFinishedAllWaves() const
{
	return CurrentWaveCount >= TotalWavesToSpawn;
}

void AWarriorSurvivalGameMode::PreLoadNextWaveEnemies()
{
	if (HasFinishedAllWaves())
	{
		return;
	}

	PreLoadedEnemyClassMap.Empty();

	for (const FWarriorEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions)
	{
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;

		// Local copy to safely capture into the asynchronous lambda closure
		TSoftClassPtr<AWarriorEnemyCharacter> SoftClass = SpawnerInfo.SoftEnemyClassToSpawn;

		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			SoftClass.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda([this, SoftClass]()
				{
					if (UClass* LoadedEnemyClass = SoftClass.Get())
					{
						// Populate your internal map
						PreLoadedEnemyClassMap.Emplace(SoftClass, LoadedEnemyClass);
					}
				})
		);
	}
}

FWarriorEnemyWaveSpawnerTableRow* AWarriorSurvivalGameMode::GetCurrentWaveSpawnerTableRow() const
{
	// Offset here by + 1 so that Index 0 matches "Wave1", Index 1 matches "Wave2", etc.
	const FName RowName = FName(TEXT("Wave") + FString::FromInt(CurrentWaveCount + 1));

	FWarriorEnemyWaveSpawnerTableRow* FoundRow = EnemyWaveSpawnerDataTable->FindRow<FWarriorEnemyWaveSpawnerTableRow>(RowName, FString());

	checkf(FoundRow, TEXT("Could not find a valid row under the name %s in the data table"), *RowName.ToString());

	return FoundRow;
}

int32 AWarriorSurvivalGameMode::TrySpawnWaveEnemies()
{
	if (TargetPointsArray.IsEmpty())
	{
		UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(), TargetPointsArray);
	}

	checkf(!TargetPointsArray.IsEmpty(), TEXT("No valid target point found in level: %s for spawning enemies"), *GetWorld()->GetName());

	uint32 EnemiesSpawnedThisTime = 0;

	FActorSpawnParameters SpawnParam;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (const FWarriorEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions)
	{
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;

		// SAFE FIX: Use Find() instead of FindChecked() to prevent crashing if async load isn't finished yet
		UClass** LoadedEnemyClassPtr = PreLoadedEnemyClassMap.Find(SpawnerInfo.SoftEnemyClassToSpawn);

		// If the class hasn't finished loading yet, skip it or log a warning
		if (!LoadedEnemyClassPtr || !*LoadedEnemyClassPtr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy class %s is still loading or failed to load. Skipping spawn this frame."), *SpawnerInfo.SoftEnemyClassToSpawn.ToString());
			continue;
		}

		UClass* LoadedEnemyClass = *LoadedEnemyClassPtr;
		const int32 NumToSpawn = FMath::RandRange(SpawnerInfo.MinPerSpawnCount, SpawnerInfo.MaxPerSpawnCount);

		for (int32 i = 0; i < NumToSpawn; i++)
		{
			const int32 RandomTargetPointIndex = FMath::RandRange(0, TargetPointsArray.Num() - 1);
			const FVector SpawnOrigin = TargetPointsArray[RandomTargetPointIndex]->GetActorLocation();
			const FRotator SpawnRotation = TargetPointsArray[RandomTargetPointIndex]->GetActorForwardVector().ToOrientationRotator();

			FVector RandomLocation;
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, SpawnOrigin, RandomLocation, 400.f);

			RandomLocation += FVector(0.f, 0.f, 150.f);

			AWarriorEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AWarriorEnemyCharacter>(LoadedEnemyClass, RandomLocation, SpawnRotation, SpawnParam);

			if (SpawnedEnemy)
			{
				SpawnedEnemy->OnDestroyed.AddDynamic(this, &AWarriorSurvivalGameMode::OnEnemyDestroyed);
				EnemiesSpawnedThisTime++;
				TotalSpawnedEnemiesThisWaveCounter++;
			}

			if (!ShouldKeepSpawnEnemies())
			{
				return EnemiesSpawnedThisTime;
			}
		}
	}

	return EnemiesSpawnedThisTime;
}

bool AWarriorSurvivalGameMode::ShouldKeepSpawnEnemies() const
{
	// Should keep spawning as long as total spawned is less than target limit
	return TotalSpawnedEnemiesThisWaveCounter < GetCurrentWaveSpawnerTableRow()->TotalEnemyToSpawnThisWave;
}

void AWarriorSurvivalGameMode::OnEnemyDestroyed(AActor* DestroyedActor)
{
	CurrentSpawnedEnemiesCounter--;

	Debug::Print(FString::Printf(TEXT("CurrentSpawnedEnemiesCounter: %i, TotalSpawnedEnemiesThisWaveCounter: %i"), CurrentSpawnedEnemiesCounter, TotalSpawnedEnemiesThisWaveCounter	));
	
	if (ShouldKeepSpawnEnemies())
	{
		CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();
	}
	else if (CurrentSpawnedEnemiesCounter == 0)
	{
		TotalSpawnedEnemiesThisWaveCounter = 0;
		CurrentSpawnedEnemiesCounter = 0;
		SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState::WaveCompleted);
	}
}

void AWarriorSurvivalGameMode::RegisterSpawnedEnemies(const TArray<AWarriorEnemyCharacter*>& InEnemiesToRegister)
{
	for (AWarriorEnemyCharacter* SpawnedEnemy : InEnemiesToRegister)
	{
		if (SpawnedEnemy)
		{
			CurrentSpawnedEnemiesCounter++;

			SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnEnemyDestroyed);
		}
	}
}
