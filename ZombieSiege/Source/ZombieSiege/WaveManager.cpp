// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveManager.h"
#include "UnitBase.h"
#include "UnitManager.h"
#include "ZombieSiegeUtils.h"
#include "UnitAiController.h"
#include "ZombieSiegePlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWaveManager::AWaveManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWaveManager::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), spawnPositionMarkerClass, spawnPositionMarkers);
	check(spawnPositionMarkers.Num() > 0);
	
	float waveInterval = CalculateWaveInterval();
	check(waveInterval > 0);
	GetWorld()->GetTimerManager().SetTimer(waveTimerHandle, this, &AWaveManager::OnWaveTimerExpiredHandler, waveInterval, false, waveInterval);
}

void AWaveManager::OnWaveTimerExpiredHandler()
{
	float waveStrengthFloat = CalculateWaveStrengh();

	SpawnWaveInternal(waveStrengthFloat);

	float waveInterval = CalculateWaveInterval();

	UE_LOG(LogTemp, Display, TEXT("[WaveManager] Spawned wave with strength %3.2f. Next wave in %3.2f seconds"), waveStrengthFloat, waveInterval);

	check(waveInterval > 0);
	GetWorld()->GetTimerManager().SetTimer(waveTimerHandle, this, &AWaveManager::OnWaveTimerExpiredHandler, waveInterval, false, waveInterval);
}

void AWaveManager::SpawnWaveInternal(float waveStrength)
{
	UWorld* world = GetWorld();
	AUnitManager* manager = AUnitManager::GetInstance(GetWorld());
	check(manager);

	TMap<FName, int> waveClasses = GetRandomUnitClassSet(waveStrength);

	FString message = "";

	const int maxErrors = 3;
	int errorCounter = 0;

	for (auto& waveClassPair : waveClasses)
	{
		FName name = waveClassPair.Key;
		int number = waveClassPair.Value;

		message += FString::Printf(TEXT("%s (%d), "), *name.ToString(), number);

		for (int i = 0; i < number; i++)
		{
			TSubclassOf<AUnitBase> unitClass = manager->GetUnitClass(name);
			check(unitClass);

			AUnitBase* unit = SpawnUnit(unitClass);

			if (!unit)
			{
				continue;
			}

			bool ordersIssued = IssueOrders(unit);
						
			if (!ordersIssued)
			{
				unit->Destroy();				
				if (errorCounter < maxErrors)
				{
					i--;
				}
				errorCounter++;
			}
		}
	}

	UE_LOG(LogTemp, Display, TEXT("[WaveManager] wave composition: %s"), *message);
}

float AWaveManager::CalculateWaveInterval()
{
	check(waveIntervalCurve);

	float difficultyNormalized = GetNormalizedDifficulty();
	return waveIntervalCurve->GetFloatValue(difficultyNormalized);
}

float AWaveManager::CalculateWaveStrengh()
{
	check(waveStrengthCurve);

	float difficultyNormalized = GetNormalizedDifficulty();
	return waveStrengthCurve->GetFloatValue(difficultyNormalized);
}

TMap<FName, int> AWaveManager::GetRandomUnitClassSet(float waveStrength)
{
	TMap<FName, int> result;

	float accumulator = 0;

	TArray<FName> keys;
	waveUnitCostMap.GetKeys(keys);

	if (keys.Num() == 0)
	{
		return result;
	}

	while (accumulator < waveStrength)
	{
		FName randKey = keys[FMath::RandRange(0, keys.Num() - 1)];
		float cost = waveUnitCostMap[randKey];
		check(cost > 0);
		accumulator += cost;

		if (result.Contains(randKey))
		{
			result[randKey]++;
		}
		else
		{
			int32& value = result.Add(randKey);
			value = 1;
		}
	}

	return result;
}

float AWaveManager::GetNormalizedDifficulty()
{
	check(maxDifficulty > 0);
	return currentDifficulty / maxDifficulty;
}

AUnitBase* AWaveManager::SpawnUnit(TSubclassOf<AUnitBase> unitClass)
{
	AActor* spawnMarker = spawnPositionMarkers[FMath::RandRange(0, spawnPositionMarkers.Num() - 1)];
	FVector randomPoint = spawnMarker->GetActorLocation();

	FVector spawnPoint;
	bool foundPoint = UZombieSiegeUtils::GetRandomReachableLocation(GetWorld(), randomPoint, spawnPointSearchRadius, spawnPoint);

	if (!foundPoint)
	{
		return nullptr;
	}

	randomPoint.Z = 200;

	AUnitBase* spawnedUnit = GetWorld()->SpawnActor<AUnitBase>(unitClass, spawnPoint, FRotator::ZeroRotator);
	if (!spawnedUnit)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn wave unit at %s"), *spawnPoint.ToString());
	}
	
	return spawnedUnit;
}

bool AWaveManager::IssueOrders(AUnitBase* unit)
{
	AZombieSiegePlayerController* localPlayerContoller = Cast<AZombieSiegePlayerController>(GetWorld()->GetFirstPlayerController());
	AUnitAiController* controller = unit->GetController<AUnitAiController>();

	int playerUnitIterator = 0;
	FAttackTestParameters attackTestParams(EAttackTestFlags::Affiliation | EAttackTestFlags::Reachability);

	if (controller)
	{
		TArray<AUnitBase*> localPlayerUnits = localPlayerContoller->GetControlledUnits();
		AUnitBase* attackTarget = nullptr;

		for (; playerUnitIterator < localPlayerUnits.Num(); playerUnitIterator++)
		{
			if (unit->CanAttackTarget(localPlayerUnits[playerUnitIterator], attackTestParams))
			{
				attackTarget = localPlayerUnits[playerUnitIterator];
				break;
			}
		}

		if (playerUnitIterator >= localPlayerUnits.Num())
		{
			playerUnitIterator = 0;
		}

		if (attackTarget)
		{
			controller->IssueAttackOnMoveOrder(attackTarget->GetActorLocation());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[WaveManager] Local player has no attackable units!"));
			return false;
		}
	}
	else if (!controller)
	{
		UE_LOG(LogTemp, Error, TEXT("[WaveManager] Unit %s has no AIController attached!"), *unit->GetName());
		checkNoEntry();
	}

	return true;
}

void AWaveManager::SpawnWave(float difficulty)
{
	SpawnWaveInternal(difficulty);
}

// Called every frame
void AWaveManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (currentDifficulty < maxDifficulty)
	{
		currentDifficulty += DeltaTime * difficultyGrowthFactor;
	}

	if(currentDifficulty > maxDifficulty)
	{
		currentDifficulty = maxDifficulty;
	}
	

}

void AWaveManager::SetDifficulty(float difficulty)
{
	currentDifficulty = difficulty;
}

float AWaveManager::GetDifficulty()
{
	return currentDifficulty;
}
