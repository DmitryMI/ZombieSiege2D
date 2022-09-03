// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieSiegeUtils.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Kismet/KismetSystemLibrary.h"


AUnitBase* UZombieSiegeUtils::FindClosestAliveUnitInRadius(
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

	AUnitBase* closest = nullptr;
	float closestDistance = 999999999.0f;

	TArray<AUnitBase*> aliveUnits = FindAliveUnitsInRadius(world, center, searchRadius, collisionChannel, unitClass, ignoredActors);

	for (AUnitBase* unit : aliveUnits)
	{
		if (closest == nullptr)
		{
			closest = unit;
			closestDistance = FVector::DistSquared2D(unit->GetActorLocation(), center);
			continue;
		}

		float distanceSqr = FVector::DistSquared2D(unit->GetActorLocation(), center);
		if (distanceSqr < closestDistance)
		{
			closest = unit;
			closestDistance = distanceSqr;
		}
	}

	return closest;
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

float UZombieSiegeUtils::GetDistance2DBetweenSimpleCollisions(AActor* actor1, AActor* actor2)
{
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

	FVector boxMin = FVector(boxCenter.X - unitRadius, boxCenter.Y - unitRadius, 0);
	FVector boxMax = FVector(boxCenter.X + unitRadius, boxCenter.Y + unitRadius, 0);
	
	FBox box(boxMin, boxMax);

	bool hasData = NavSys->ContainsNavData(box);

	return hasData;
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

	if (IsUnitOnNavMeshWorld(world, goalAgent))
	{
		UNavigationPath* navPath = FindPathBetweenLocationsWorld(world, movingActorLocation, targetActorLocation, movingAgent);
		if (navPath == nullptr || !navPath->IsValid())
		{
			return false;
		}

		FVector lastPoint = navPath->PathPoints.Last();
		OutLocation = lastPoint;

		if (!navPath->IsPartial())
		{
			return true;
		}
		
		FVector deltaVec = targetActorLocation - lastPoint;
		bool isCloseEnough = deltaVec.SizeSquared2D() > FMath::Square(useReachabilityRadius);

		return isCloseEnough;
	}
	else
	{
		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(world);

		FNavLocation randomPoint;
		bool foundPoint = NavSys->GetRandomPointInNavigableRadius(targetActorLocation, useReachabilityRadius, randomPoint);

		if (!foundPoint)
		{
			return false;
		}

		OutLocation = randomPoint.Location;
		return true;
	}
}
