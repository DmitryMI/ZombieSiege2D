// Copyright Epic Games, Inc. All Rights Reserved.


#include "ZombieSiegeGameModeBase.h"
#include "UnitBase.h"
#include "Zombie.h"
#include "UnitManager.h"
#include "ZombieAiController.h"
#include "ZombieSiegeUtils.h"

void AZombieSiegeGameModeBase::SpawnDebugHorde(int count)
{
	UWorld* world = GetWorld();
	AUnitManager* manager = AUnitManager::GetInstance(world);
	check(manager);

	TSubclassOf<AUnitBase> unitClass = manager->GetUnitClass("Zombie");

	if (!unitClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Zombie UnitClass is not registered!"));
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

	for (int i = 0; i < count; i++)
	{
	
		FVector randomPoint;
		bool foundPoint = UZombieSiegeUtils::GetRandomReachableLocation(GetWorld(), terrainLocation, count * 100.0f, randomPoint);

		if (!foundPoint)
		{
			continue;
		}

		AZombie* zombie = world->SpawnActor<AZombie>(unitClass, randomPoint, FRotator::ZeroRotator);
		if (zombie)
		{
			FString zombieName;
			zombie->GetName(zombieName);
			UE_LOG(LogTemp, Warning, TEXT("Debug zombie %s spawned at %s"), *zombieName, *zombie->GetActorLocation().ToString());

			AZombieAiController* controller = zombie->GetController<AZombieAiController>();
			check(controller);

			TArray<AUnitBase*> localPlayerUnits = localPlayerContoller->GetControlledUnits();

			if (localPlayerUnits.Num() != 0)
			{
				AUnitBase* randomPlayersUnit = localPlayerUnits[FMath::RandRange(0, localPlayerUnits.Num() - 1)];
				controller->IssueAttackOnMoveOrder(randomPlayersUnit->GetActorLocation());
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn debug zombie at %s"), *randomPoint.ToString());
		}
	}
}
