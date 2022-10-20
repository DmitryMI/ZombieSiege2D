// Copyright Epic Games, Inc. All Rights Reserved.


#include "ZombieSiegeGameModeBase.h"
#include "UnitBase.h"
#include "Zombie.h"
#include "UnitManager.h"
#include "ZombieAiController.h"
#include "ZombieSiegeUtils.h"

void AZombieSiegeGameModeBase::SpawnDebugHorde(FName unitName, int count, bool bIssueAttackOnMoveOrder)
{
	UWorld* world = GetWorld();
	AUnitManager* manager = AUnitManager::GetInstance(world);
	check(manager);

	TSubclassOf<AUnitBase> unitClass = manager->GetUnitClass(unitName);

	if (!unitClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UnitClass %s is not registered!"), *unitName.ToString());
		return;
	}

	AZombieSiegePlayerController* localPlayerContoller = Cast<AZombieSiegePlayerController>(world->GetFirstPlayerController());

	check(localPlayerContoller);

	int viewportWidth, viewportHeight;
	localPlayerContoller->GetViewportSize(viewportWidth, viewportHeight);

	FVector worldLocation;
	FVector worldDirection;

	bool deprojected = localPlayerContoller->DeprojectScreenPositionToWorld(viewportWidth / 2, viewportHeight / 2, worldLocation, worldDirection);

	if (!deprojected)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: failed to deproject player's screen center."), __FUNCTION__);
		return;
	}

	FVector terrainLocation = UZombieSiegeUtils::GetTerrainIntersection(worldLocation, worldDirection, terrainHeight);

	float radius = FMath::Min(viewportWidth, viewportHeight);

	for (int i = 0; i < count; i++)
	{
	
		FVector randomPoint;
		bool foundPoint = UZombieSiegeUtils::GetRandomReachableLocation(GetWorld(), terrainLocation, radius, randomPoint);

		if (!foundPoint)
		{
			continue;
		}

		randomPoint.Z = 250;

		AUnitBase* spawnedUnit = world->SpawnActor<AUnitBase>(unitClass, randomPoint, FRotator::ZeroRotator);
		if (spawnedUnit)
		{
			FString unitName;
			spawnedUnit->GetName(unitName);
			UE_LOG(LogTemp, Warning, TEXT("Debug unit %s spawned at %s"), *unitName, *spawnedUnit->GetActorLocation().ToString());

			AUnitAiController* controller = spawnedUnit->GetController<AUnitAiController>();
			
			if (controller && bIssueAttackOnMoveOrder)
			{
				TArray<AUnitBase*> localPlayerUnits = localPlayerContoller->GetControlledUnits();

				if (localPlayerUnits.Num() != 0)
				{
					AUnitBase* randomPlayersUnit = localPlayerUnits[FMath::RandRange(0, localPlayerUnits.Num() - 1)];
					check(randomPlayersUnit);
					controller->IssueAttackOnMoveOrder(randomPlayersUnit->GetActorLocation());
				}
			}
			else if(!controller)
			{
				UE_LOG(LogTemp, Error, TEXT("Unit has no AIController attached!"), *randomPoint.ToString());
			}
			
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn debug unit at %s"), *randomPoint.ToString());
		}
	}
}

void AZombieSiegeGameModeBase::SpawnDebugZombies(int count, bool bIssueAttackOnMoveOrder)
{
	SpawnDebugHorde("Zombie", count, bIssueAttackOnMoveOrder);
}
