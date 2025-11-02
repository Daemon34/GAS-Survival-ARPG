// Clement Lecoeur All Rights Reserved


#include "GameModes/SARPGSurvivalGameMode.h"
#include "Engine/AssetManager.h"
#include "Characters/SARPGEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"
#include "NavigationSystem.h"
#include "SARPGFunctionLibrary.h"

#include "SARPGDebugHelper.h"

void ASARPGSurvivalGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	ESARPGGameDifficulty SavedCurrentGameDifficulty;
	if (USARPGFunctionLibrary::TryLoadSavedGameDifficulty(SavedCurrentGameDifficulty)) {
		CurrentGameDifficulty = SavedCurrentGameDifficulty;
	}
}

void ASARPGSurvivalGameMode::BeginPlay()
{
	Super::BeginPlay();

	checkf(EnemyWaveSpawnerDataTable, TEXT("Forgot to assign a valid data table to the Survival Game Mode Blueprint"));

	SetCurrentSurvivalGameModeState(ESARPGSurvivalGameModeState::WaitSpawnNewWave);

	TotalWavesToSpawn = EnemyWaveSpawnerDataTable->GetRowNames().Num();

	PreLoadNextWaveEnemies();
}

void ASARPGSurvivalGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentSurvivalGameModeState == ESARPGSurvivalGameModeState::WaitSpawnNewWave) {
		TimePassedSinceStart += DeltaTime;
		if(TimePassedSinceStart >= SpawnNewWaveWaitTime) {
			TimePassedSinceStart = 0.0f;

			SetCurrentSurvivalGameModeState(ESARPGSurvivalGameModeState::SpawningNewWave);
		}
	}

	if (CurrentSurvivalGameModeState == ESARPGSurvivalGameModeState::SpawningNewWave) {
		TimePassedSinceStart += DeltaTime;
		if (TimePassedSinceStart >= SpawnEnemiesDelayTime) {
			CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();

			TimePassedSinceStart = 0.0f;

			SetCurrentSurvivalGameModeState(ESARPGSurvivalGameModeState::InProgress);
		}
	}

	if(CurrentSurvivalGameModeState == ESARPGSurvivalGameModeState::WaveCompleted) {
		TimePassedSinceStart += DeltaTime;

		if(TimePassedSinceStart >= WaveCompletedWaitTime) {
			TimePassedSinceStart = 0.0f;

			CurrentWaveCount++;

			if (HasFinishedAllWaves()) {
				SetCurrentSurvivalGameModeState(ESARPGSurvivalGameModeState::AllWavesDone);
			}
			else {
				SetCurrentSurvivalGameModeState(ESARPGSurvivalGameModeState::WaitSpawnNewWave);
				PreLoadNextWaveEnemies();
			}
		}
	}
}

void ASARPGSurvivalGameMode::SetCurrentSurvivalGameModeState(ESARPGSurvivalGameModeState InState)
{
	CurrentSurvivalGameModeState = InState;
	OnSurvivalGameModeStateChanged.Broadcast(CurrentSurvivalGameModeState);
}

bool ASARPGSurvivalGameMode::HasFinishedAllWaves() const
{
	return CurrentWaveCount > TotalWavesToSpawn;
}

void ASARPGSurvivalGameMode::PreLoadNextWaveEnemies()
{
	if (HasFinishedAllWaves()) {
		return;
	}

	PreLoadedEnemyClassMap.Empty();

	for(const FSARPGEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions) {
		if(SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) {
			continue;
		}

		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			SpawnerInfo.SoftEnemyClassToSpawn.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda([SpawnerInfo, this]() {
				if (UClass* LoadedEnemyClass = SpawnerInfo.SoftEnemyClassToSpawn.Get()) {
					PreLoadedEnemyClassMap.Emplace(SpawnerInfo.SoftEnemyClassToSpawn, LoadedEnemyClass);
				}
			})
		);
	}
}

FSARPGEnemyWaveSpawnerTableRow* ASARPGSurvivalGameMode::GetCurrentWaveSpawnerTableRow() const
{
	const FName RowName = FName(TEXT("Wave") + FString::FromInt(CurrentWaveCount));

	FSARPGEnemyWaveSpawnerTableRow* FoundRow = EnemyWaveSpawnerDataTable->FindRow<FSARPGEnemyWaveSpawnerTableRow>(RowName, FString());

	checkf(FoundRow, TEXT("Could not find wave spawner data table row for wave %s"), *RowName.ToString());

	return FoundRow;
}

int32 ASARPGSurvivalGameMode::TrySpawnWaveEnemies()
{
	if (TargetPointsArray.IsEmpty()) {
		UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(), TargetPointsArray);
	}

	checkf(!TargetPointsArray.IsEmpty(), TEXT("No Target Points found in the level '%s' to spawn enemies at"), *GetWorld()->GetName());

	uint32 EnemiesSpawnedThisTime = 0;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (const FSARPGEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions) {
		if(SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) {
			continue;
		}

		const int32 NumToSpawn = FMath::RandRange(SpawnerInfo.MinPerSpawnCount, SpawnerInfo.MaxPerSpawnCount);

		UClass* LoadedEnemyClass = PreLoadedEnemyClassMap.FindChecked(SpawnerInfo.SoftEnemyClassToSpawn);

		for (int32 i = 0; i < NumToSpawn; i++) {
			const int32 RandomTargetPointIndex = FMath::RandRange(0, TargetPointsArray.Num() - 1);
			const FVector SpawnOrigin = TargetPointsArray[RandomTargetPointIndex]->GetActorLocation();
			const FRotator SpawnRotation = TargetPointsArray[RandomTargetPointIndex]->GetActorForwardVector().ToOrientationRotator();

			FVector RandomLocation;

			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(
				this,
				SpawnOrigin,
				RandomLocation,
				400.0f
			);

			RandomLocation += FVector(0.0f, 0.0f, 150.0f); // To avoid spawning in the ground

			ASARPGEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<ASARPGEnemyCharacter>(
				LoadedEnemyClass,
				RandomLocation,
				SpawnRotation,
				SpawnParams
			);

			if(SpawnedEnemy){
				SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this, &ASARPGSurvivalGameMode::OnEnemyDestroyed);

				EnemiesSpawnedThisTime++;
				TotalSpawnedEnemiesThisWaveCounter++;
			}

			if(!ShouldKeepSpawnEnemies()) {
				return EnemiesSpawnedThisTime;
			}
		}
	}

	return EnemiesSpawnedThisTime;
}

bool ASARPGSurvivalGameMode::ShouldKeepSpawnEnemies() const
{
	return TotalSpawnedEnemiesThisWaveCounter < GetCurrentWaveSpawnerTableRow()->TotalEnemyToSpawnThisWave;
}

void ASARPGSurvivalGameMode::OnEnemyDestroyed(AActor* DestroyedActor)
{
	CurrentSpawnedEnemiesCounter--;

	if (ShouldKeepSpawnEnemies()) {
		CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();
	}
	else if (CurrentSpawnedEnemiesCounter == 0) {
		TotalSpawnedEnemiesThisWaveCounter = 0;
		CurrentSpawnedEnemiesCounter = 0;

		SetCurrentSurvivalGameModeState(ESARPGSurvivalGameModeState::WaveCompleted);
	}
}

void ASARPGSurvivalGameMode::RegisterSpawnedEnemies(const TArray<ASARPGEnemyCharacter*>& InEnemiesToRegister)
{
	for(ASARPGEnemyCharacter* SpawnedEnemy : InEnemiesToRegister) {
		if(SpawnedEnemy) {
			SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnEnemyDestroyed);
			CurrentSpawnedEnemiesCounter++;
		}
	}
}
