// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieSiegeUtils.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Kismet/KismetSystemLibrary.h"


int UZombieSiegeUtils::CompareDistances2D(AActor* actorA, AActor* actorB, AActor* pivotActor)
{
	FVector pivot = pivotActor->GetActorLocation();

	float distA = FVector::DistSquared2D(actorA->GetActorLocation(), pivot);
	float distB = FVector::DistSquared2D(actorB->GetActorLocation(), pivot);

	if (distA < distB)
	{
		return -1;
	}
	else if (distA > distB)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

AUnitBase* UZombieSiegeUtils::GetClosestUnit(const TArray<AUnitBase*>& actors, FVector location)
{
	return GetClosestActor(actors, location);
}

TArray<AUnitBase*> UZombieSiegeUtils::FindAliveUnitsInRadius(
	const UObject* WorldContextObject,
	const FVector& center,
	float searchRadius,
	ECollisionChannel collisionChannel,
	TSubclassOf<AUnitBase> unitClass,
	const TArray<AActor*>& ignoredActors
)
{
	UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	check(world);

	TArray<AUnitBase*> units;
	TArray<AActor*> actorsInRadius;

	EObjectTypeQuery objectType = UEngineTypes::ConvertToObjectType(collisionChannel);

	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes{ objectType };

	bool foundAny = UKismetSystemLibrary::SphereOverlapActors(world, center, searchRadius, objectTypes, unitClass.Get(), ignoredActors, actorsInRadius);

	if (foundAny)
	{
		for (AActor* actor : actorsInRadius)
		{
			AUnitBase* unit = Cast<AUnitBase>(actor);
			check(unit);

			if (!unit->IsAlive())
			{
				continue;
			}

			units.Add(unit);
		}
	}

	return units;
}

TArray<AUnitBase*> UZombieSiegeUtils::FindAttackableEnemiesInRadius(const UObject* WorldContextObject, AUnitBase* instigator, const FVector& center, float searchRadius, const TArray<AActor*>& ignoredActors)
{
	auto ignoredActorsClone = ignoredActors;
	if (!ignoredActorsClone.Contains(instigator))
	{
		ignoredActorsClone.Add(instigator);
	}

	UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	check(world);

	auto filter = [instigator, world](AUnitBase* unit)
	{
		if (!UZombieSiegeUtils::AreEnemies(instigator, unit))
		{
			return false;
		}

		if (!unit->IsAlive())
		{
			return false;
		}

		if (!instigator->CanEverAttackTarget(unit))
		{
			return false;
		}

		if (!UZombieSiegeUtils::IsUnitReachable(world, instigator, unit, 250.0f))
		{
			return false;
		}

		return true;
	};

	TArray<AUnitBase*> units = FindUnitsInRadius<AUnitBase>(world, center, searchRadius, ignoredActors, filter);

	return units;
}


float UZombieSiegeUtils::GetDistance2DBetweenSimpleCollisions(AActor* actor1, AActor* actor2)
{
	check(actor1);
	check(actor2);

	FVector actor1Center = actor1->GetActorLocation();
	FVector actor2Center = actor2->GetActorLocation();
	float distanceBetweenCenters = (actor2Center - actor1Center).Size2D();

	float actorRadius1;
	float actorHalfHeight1;
	actor1->GetSimpleCollisionCylinder(actorRadius1, actorHalfHeight1);

	float actorRadius2;
	float actorHalfHeight2;
	actor2->GetSimpleCollisionCylinder(actorRadius2, actorHalfHeight2);

	float distance =  distanceBetweenCenters - actorRadius1 - actorRadius2;

	return distance;
}

bool UZombieSiegeUtils::IsLocationReachable(const UObject* WorldContextObject, AUnitBase* unit, FVector Position, float tolerance)
{
	UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	return IsLocationReachableWorld(world, unit, Position, tolerance);
}

bool UZombieSiegeUtils::IsLocationReachableWorld(UWorld* world, AUnitBase* unit, FVector position, float tolerance)
{
	check(unit);
	check(world);

	FVector pathStart = unit->GetActorLocation();
	UNavigationPath* navPath = FindPathBetweenLocationsWorld(world, pathStart, position, unit);
	if (!navPath)
	{
		return false;
	}

	if (!navPath->IsPartial())
	{
		return true;
	}

	if (navPath->PathPoints.Num() == 0)
	{
		return false;
	}

	FVector deltaVec = position - navPath->PathPoints.Last();
	if (deltaVec.SizeSquared2D() > FMath::Square(tolerance))
	{
		return false;
	}

	return true;
}

bool UZombieSiegeUtils::IsUnitReachable(const UObject* WorldContextObject, AUnitBase* agent, AUnitBase* targetUnit, float tolerance)
{
	FVector unused;
	bool reachable = GetBestLocationNearUnitToArrive(WorldContextObject, agent, targetUnit, tolerance, unused);
	return reachable;
}

bool UZombieSiegeUtils::IsUnitReachable(UWorld* world, AUnitBase* agent, AUnitBase* targetUnit, float tolerance)
{
	FVector unused;
	bool reachable = GetBestLocationNearUnitToArriveWorld(world, agent, targetUnit, tolerance, unused);
	return reachable;
}

UNavigationPath* UZombieSiegeUtils::FindPathBetweenLocations(const UObject* WorldContextObject, FVector from, FVector to, AActor* pathFindingContext)
{
	UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	return FindPathBetweenLocationsWorld(world, from, to, pathFindingContext);
}

UNavigationPath* UZombieSiegeUtils::FindPathBetweenLocationsWorld(UWorld* world, FVector from, FVector to, AActor* pathFindingContext)
{
	UNavigationPath* navPath = UNavigationSystemV1::FindPathToLocationSynchronously(world, from, to, pathFindingContext);
	return navPath;
}

bool UZombieSiegeUtils::IsUnitOnNavMeshWorld(UWorld* world, AUnitBase* unit)
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(world);

	float unitRadius;
	float unitHalfHeight;
	unit->GetSimpleCollisionCylinder(unitRadius, unitHalfHeight);

	FVector boxCenter = unit->GetActorLocation();

	boxCenter.Z = 0;

	FVector boxMin = FVector(boxCenter.X - unitRadius, boxCenter.Y - unitRadius, -200);
	FVector boxMax = FVector(boxCenter.X + unitRadius, boxCenter.Y + unitRadius, 200);
	
	FBox box(boxMin, boxMax);

	bool hasData = NavSys->ContainsNavData(box);

	return hasData;
}

bool UZombieSiegeUtils::GetFirstPointCloseToGoal(FVector goalLocation, float reachabilityRadius, UNavigationPath* navPath, FVector& result)
{
	for (const FVector& location : navPath->PathPoints)
	{
		FVector deltaVec = goalLocation - location;

		bool isCloseEnough;
#if UE_BUILD_SHIPPING
		isCloseEnough = deltaVec.SizeSquared2D() <= FMath::Square(reachabilityRadius);
#else
		float deltaLength = deltaVec.Size();
		isCloseEnough = deltaLength <= reachabilityRadius;
#endif
		if (isCloseEnough)
		{
			result = location;
			return true;
		}

	}
	result = navPath->PathPoints.Last();
	return false;
}

bool UZombieSiegeUtils::GetBestLocationNearUnitToArrive(
	const UObject* WorldContextObject,
	AUnitBase* movingAgent,
	AUnitBase* targetAgent,
	float tolerance,
	FVector& OutLocation)
{
	UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	return GetBestLocationNearUnitToArriveWorld(world, movingAgent, targetAgent, tolerance, OutLocation);
}

bool UZombieSiegeUtils::GetBestLocationNearUnitToArriveWorld(
	UWorld* world,
	AUnitBase* movingAgent,
	AUnitBase* goalAgent,
	float tolerance,
	FVector& OutLocation)
{
	check(world);
	check(goalAgent);
	check(movingAgent);

	float movingAgentRadius;
	float movingAgentHalfHeight;
	float goalAgentRadius;
	float goalAgentHalfHeight;

	movingAgent->GetSimpleCollisionCylinder(movingAgentRadius, movingAgentHalfHeight);
	goalAgent->GetSimpleCollisionCylinder(goalAgentRadius, goalAgentHalfHeight);

	float useReachabilityRadius = tolerance + movingAgentRadius + goalAgentRadius;

	FVector movingActorLocation = movingAgent->GetActorLocation();
	FVector targetActorLocation = goalAgent->GetActorLocation();

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(world);

	//UNavigationPath* navPath = FindPathBetweenLocationsWorld(world, movingActorLocation, targetActorLocation, movingAgent);
	UNavigationPath* pathToActor = NavSys->FindPathToActorSynchronously(world, movingActorLocation, goalAgent, tolerance, movingAgent);
	pathToActor->EnableDebugDrawing(true, FLinearColor::Red);

	bool bPathToActorInvalid = !pathToActor->IsValid();
	if (pathToActor == nullptr || bPathToActorInvalid)
	{
		// Try to find path to a random point near the target instead

		FNavLocation randomPoint;
		bool foundPoint = NavSys->GetRandomPointInNavigableRadius(targetActorLocation, useReachabilityRadius, randomPoint);

		if (!foundPoint)
		{
			return false;
		}

		UNavigationPath* pathToRandomPoint = FindPathBetweenLocationsWorld(world, movingActorLocation, randomPoint.Location, movingAgent);
		pathToRandomPoint->EnableDebugDrawing(true, FLinearColor::Green);
		bool pathToRandomPointInvalid = !pathToRandomPoint->IsValid();
		if (pathToRandomPoint == nullptr || pathToRandomPointInvalid)
		{
			return false;
		}		

		bool hasPoint = GetFirstPointCloseToGoal(targetActorLocation, useReachabilityRadius, pathToRandomPoint, OutLocation);

		return hasPoint;
	}
	else if (pathToActor->IsPartial())
	{
		bool hasPoint = GetFirstPointCloseToGoal(targetActorLocation, useReachabilityRadius, pathToActor, OutLocation);

		return hasPoint;
	}
	else // Path is valid and is not partial
	{
		OutLocation = pathToActor->PathPoints.Last();
		return true;
	}

}

bool UZombieSiegeUtils::AreEnemies(AUnitBase* unitA, AUnitBase* unitB)
{
	check(unitA);
	check(unitB);

	AZombieSiegePlayerController* controllerA = unitA->GetOwningPlayerController();
	AZombieSiegePlayerController* controllerB = unitB->GetOwningPlayerController();

	return controllerA != controllerB;
}

int UZombieSiegeUtils::GetDirectionSpriteIndex(EFaceDirection faceDirectionEnum)
{
	if (static_cast<uint8>(faceDirectionEnum) & static_cast<uint8>(EFaceDirection::Up))
	{
		return 0;
	}
	else if (static_cast<uint8>(faceDirectionEnum) & static_cast<uint8>(EFaceDirection::Down))
	{
		return 1;
	}
	else if (static_cast<uint8>(faceDirectionEnum) & static_cast<uint8>(EFaceDirection::Right))
	{
		return 2;
	}
	else if (static_cast<uint8>(faceDirectionEnum) & static_cast<uint8>(EFaceDirection::Left))
	{
		return 3;
	}
	else
	{
		return 1;
	}
}

bool UZombieSiegeUtils::IsFree(const TMap<EResourceType, float> requiredResources)
{
	float summ = 0.0f;
	for (auto resourcePair : requiredResources)
	{
		summ += resourcePair.Value;
	}

	return FMath::IsNearlyZero(summ);
}

FVector UZombieSiegeUtils::GetTerrainIntersection(const FVector& location, const FVector& direction, float terrainHeight)
{
	float rayLength = UE_LARGE_WORLD_MAX;

	FVector secondWorldPoint = location + direction * rayLength;

	FPlane plane(FVector(0, 0, terrainHeight), FVector::UpVector);
	FVector intersection = FMath::LinePlaneIntersection(location, secondWorldPoint, plane);

	return intersection;
}

bool UZombieSiegeUtils::GetRandomReachableLocation(const UObject* WorldContextObject, const FVector& center, float radius, FVector& outRandomLocation)
{
	UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	return GetRandomReachableLocation(world, center, radius, outRandomLocation);
}

bool UZombieSiegeUtils::GetRandomReachableLocation(const UWorld* world, const FVector& location, float radius, FVector& outRandomLocation)
{
	const UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(world);

	FNavLocation randomPoint;
	bool foundPoint = NavSys->GetRandomPointInNavigableRadius(location, radius, randomPoint);

	outRandomLocation = randomPoint.Location;

	return foundPoint;
}
