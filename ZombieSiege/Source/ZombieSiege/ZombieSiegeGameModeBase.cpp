// Copyright Epic Games, Inc. All Rights Reserved.


#include "ZombieSiegeGameModeBase.h"
#include "UnitBase.h"
#include "Zombie.h"
#include "UnitManager.h"
#include "ZombieAiController.h"
#include "ZombieSiegeUtils.h"

void AZombieSiegeGameModeBase::SpawnDebugHorde(FName unitTypeName, int count, bool bIssueAttackOnMoveOrder)
{
	UWorld* world = GetWorld();
	AUnitManager* manager = AUnitManager::GetInstance(world);
	check(manager);

	TSubclassOf<AUnitBase> unitClass = manager->GetUnitClass(unitTypeName);

	if (!unitClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UnitClass %s is not registered!"), *unitTypeName.ToString());
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

	int playerUnitIterator = 0;
	FAttackTestParameters attackTestParams(EAttackTestFlags::Affiliation | EAttackTestFlags::Reachability);

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
				AUnitBase* attackTarget = nullptr;

				for (; playerUnitIterator < localPlayerUnits.Num(); playerUnitIterator++)
				{
					if (spawnedUnit->CanAttackTarget(localPlayerUnits[playerUnitIterator], attackTestParams))
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
					UE_LOG(LogTemp, Error, TEXT("[SpawnDebugHorde] Local player has no attackable units!"));
				}
			}
			else if(!controller)
			{
				UE_LOG(LogTemp, Error, TEXT("Unit %s has no AIController attached!"), *spawnedUnit->GetName());
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

void AZombieSiegeGameModeBase::KillAll(FName unitTypeName, bool bForceDeath)
{
	// TODO Replace UGameplayStatics::GetAllActorsOfClass with something more efficient

	AUnitManager* manager = AUnitManager::GetInstance(GetWorld());
	check(manager);

	TSubclassOf<AUnitBase> unitClass = manager->GetUnitClass(unitTypeName);

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), unitClass, actors);

	for (AActor* actor : actors)
	{
		AUnitBase* unit = Cast<AUnitBase>(actor);
		if (!unit)
		{
			continue;
		}

		unit->KillUnit(bForceDeath);
	}
}
