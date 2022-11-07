// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieSiegeUtils.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NavMesh/NavMeshPath.h"
#include "NavMesh/RecastNavMesh.h"
#include "Kismet/KismetSystemLibrary.h"

#define DRAW_DEBUG_PATH 0

// This is the reason of HUGE lags during dynamic NavMesh generation.
// UNavigationPath objects are not desroyed immideatly after GetBestLocationNearUnitToArrive() returns
// That means, that the path is being observed by the NavSys, despite the fact we don't need it to do that.
// FIX: Use FNavigationPath instead.
// The buggy conditional compilation block left here for educational purposes. Do not set USE_UNAVPATH to 1 unless
// you want to bring the lags back
#define USE_UNAVPATH 0


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

TArray<AUnitBase*> UZombieSiegeUtils::FindAttackableEnemiesInRadius(const UObject* WorldContextObject, AUnitBase* instigator, const FVector& center, float searchRadius, const TArray<AActor*>& ignoredActors, bool bTestReachability)
{
	auto ignoredActorsClone = ignoredActors;
	if (!ignoredActorsClone.Contains(instigator))
	{
		ignoredActorsClone.Add(instigator);
	}

	UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	check(world);

	auto filter = [instigator, world, bTestReachability](AUnitBase* unit)
	{
		if (!UZombieSiegeUtils::AreEnemies(instigator, unit))
		{
			return false;
		}

		if (!unit->IsAlive())
		{
			return false;
		}

		if (!instigator->CanAttackTarget(unit, FAttackTestParameters(false, false, false, false)))
		{
			return false;
		}

		if (bTestReachability && !UZombieSiegeUtils::IsUnitReachable(world, instigator, unit, 250.0f))
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

void UZombieSiegeUtils::DebugDrawPath(const UObject* WorldContextObject, FNavigationPath* navPath, FVector start, FVector goal, float duration, float zOffset)
{
	const uint8 depthPriority = 0;
	const FVector drawingOffset = FVector(0, 0, zOffset);
	const float maxCost = 1000000.0f;

	UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	//FNavigationPath* navPath = path->GetPath().Get();
	FNavMeshPath* navMeshPath = (FNavMeshPath*)(navPath);

	if (!navMeshPath)
	{
		return;
	}

	auto points = navMeshPath->GetPathPoints();

	if (points.Num() == 0)
	{
		return;
	}

	FNavPathPoint lastPoint = points[0];

	for (int i = 1; i < points.Num(); i++)
	{
		DrawDebugDirectionalArrow(world, lastPoint.Location + drawingOffset, points[i].Location + drawingOffset, 100.0f, FColor::Green, false, duration, depthPriority, 2.0f);
		lastPoint = points[i];
	}

	TArray<uint64> corridorNodes = navMeshPath->PathCorridor;
	TArray<double> corridorCost = navMeshPath->PathCorridorCost;
	TArray<FNavigationPortalEdge> corridorEdges = navMeshPath->GetPathCorridorEdges();

	if (corridorEdges.Num() == 0)
	{
		return;
	}

	FNavigationPortalEdge lastEdge = corridorEdges[0];

	for (int i = 1; i < corridorEdges.Num(); i++)
	{
		float costRatio = FMath::Clamp((float)corridorCost[i] / maxCost, 0.0f, 1.0f);

		float red = FMath::Lerp(0, 255, costRatio);
		float blue = FMath::Lerp(255, 0, costRatio);
		float green = 0.0f;

		FColor lineColor(red, green, blue, 255);
		
		FNavigationPortalEdge edge = corridorEdges[i];
		DrawDebugLine(world, lastEdge.Left + drawingOffset, edge.Left + drawingOffset, lineColor, false, duration, depthPriority, 2.0f);
		DrawDebugLine(world, lastEdge.Right + drawingOffset, edge.Right + drawingOffset, lineColor, false, duration, depthPriority, 2.0f);
		DrawDebugLine(world, edge.Left + drawingOffset, edge.Right + drawingOffset, FColor::Black, false, duration, depthPriority, 1.0f);

		lastEdge = edge;
	}

	uint64 goalPolyIndex = corridorNodes.Last();

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(world);
	ARecastNavMesh* navData = Cast<ARecastNavMesh>(NavSys->MainNavData.Get());
	check(navData);

	TArray<FNavigationPortalEdge> goalPolyEdges;
	verify(navData->GetPolyEdges(goalPolyIndex, goalPolyEdges));
	
	float goalAreaCost = (float)corridorCost.Last();

	float goalAreaCostRatio = FMath::Clamp(goalAreaCost / maxCost, 0.0f, 1.0f);

	float goalRed = FMath::Lerp(0, 255, goalAreaCostRatio);
	float goalBlue = FMath::Lerp(255, 0, goalAreaCostRatio);
	float goalGreen = 0.0f;

	FColor goalPolyColor = FColor(goalRed, goalGreen, goalBlue, 100);

	for (int i = 1; i < goalPolyEdges.Num(); i++)
	{
		DrawDebugLine(world, goalPolyEdges[i].Left + drawingOffset, goalPolyEdges[i - 1].Left + drawingOffset, goalPolyColor, false, duration, depthPriority, 2.0f);
		DrawDebugLine(world, goalPolyEdges[i].Right + drawingOffset, goalPolyEdges[i - 1].Right + drawingOffset, goalPolyColor, false, duration, depthPriority, 2.0f);
	}
	DrawDebugLine(world, goalPolyEdges[0].Left + drawingOffset, goalPolyEdges.Last().Left + drawingOffset, goalPolyColor, false, duration, depthPriority, 2.0f);
	DrawDebugLine(world, goalPolyEdges[0].Right + drawingOffset, goalPolyEdges.Last().Right + drawingOffset, goalPolyColor, false, duration, depthPriority, 2.0f);
}


bool UZombieSiegeUtils::GetBestLocationNearUnitToArrive(
	const UObject* WorldContextObject,
	AActor* movingAgent,
	AActor* targetAgent,
	float tolerance,
	FVector& OutLocation)
{
	UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	return GetBestLocationNearUnitToArriveWorld(world, movingAgent, targetAgent, tolerance, OutLocation);
}

bool UZombieSiegeUtils::GetBestLocationNearUnitToArriveWorld(
	UWorld* world,
	AActor* movingAgent,
	AActor* goalAgent,
	float tolerance,
	FVector& OutLocation)
{
	const float MaxPolygonCost = 1000000.0f;

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

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(world);
	ARecastNavMesh* NavigationData = Cast<ARecastNavMesh>(NavSys->MainNavData.Get());
	check(NavigationData);

	FVector movingActorLocation = movingAgent->GetActorLocation();
	FVector targetActorLocation = goalAgent->GetActorLocation();

	//FVector movingActorLocationProjected = NavSys->ProjectPointToNavigation(world, movingActorLocation, NavigationData, nullptr, FVector(0, 0, 2000));
	//FVector targetActorLocationProjected = NavSys->ProjectPointToNavigation(world, targetActorLocation, NavigationData, nullptr, FVector(0, 0, 2000));
	FNavLocation movingActorNavLocation;
	bool bMovingActorProjected = NavSys->ProjectPointToNavigation(movingActorLocation, movingActorNavLocation, FVector(0, 0, 2000), NavigationData);
	if (!bMovingActorProjected)
	{
		return false;
	}

	FVector movingActorLocationProjected = movingActorNavLocation.Location;

	FNavLocation targetActorNavLocation;
	bool bTargetActorProjected = NavSys->ProjectPointToNavigation(targetActorLocation, targetActorNavLocation, FVector(0, 0, 2000), NavigationData);
	if (!bTargetActorProjected)
	{
		return false;
	}

	FVector targetActorLocationProjected = targetActorNavLocation.Location;

#if USE_UNAVPATH
	UNavigationPath* path = NavSys->FindPathToActorSynchronously(world, movingActorLocationProjected, goalAgent, 50.0f, movingAgent);
	FNavigationPath* navPath = path->GetPath().Get();
#else
	INavAgentInterface* NavAgent = Cast<INavAgentInterface>(movingAgent);

	const FPathFindingQuery Query(
		movingAgent,
		*NavigationData,
		movingActorLocationProjected,
		targetActorLocationProjected,
		UNavigationQueryFilter::GetQueryFilter(*NavigationData, movingAgent, 0));
	const FPathFindingResult Result = NavSys->FindPathSync(Query, EPathFindingMode::Regular);

	FNavigationPath* navPath = Result.Path.Get();
#endif

	if (!navPath || !navPath->IsValid())
	{
		return false;
	}

#if WITH_EDITOR && DRAW_DEBUG_PATH
	DebugDrawPath(world, navPath, movingActorLocation, targetActorLocation, 0.5f, 100.0f);
#endif

	FNavMeshPath* navMeshPath = (FNavMeshPath*)(navPath);
	check(navMeshPath);	

	TArray<uint64> corridor = navMeshPath->PathCorridor;
	TArray<double> corridorCosts = navMeshPath->PathCorridorCost;
	for (int i = 0; i < corridor.Num() - 1; i++)
	{
		uint64 polyId = corridor[i];
		float areaCost = corridorCosts[i];
		if (areaCost >= MaxPolygonCost)
		{
			return false;
		}
	}

	uint64 goalPolyId = corridor.Last();
	float goalPolyCost = corridorCosts.Last();
	if (goalPolyCost < MaxPolygonCost)
	{
		OutLocation = navPath->GetPathPoints().Last().Location;
		return true;
	}

	FVector closestAccessiblePoint;
	if (corridor.Num() >= 2)
	{
		uint64 lastAccessiblePolyId = corridor[corridor.Num() - 2];
		closestAccessiblePoint;
		bool hasPoint = NavigationData->GetClosestPointOnPoly(lastAccessiblePolyId, targetActorLocationProjected, closestAccessiblePoint);
		if (!hasPoint)
		{
			return false;
		}
	}
	else
	{
		closestAccessiblePoint = navPath->GetPathPoints().Last().Location;
	}
	
	OutLocation = closestAccessiblePoint;

	float distanceSqr = (targetActorLocationProjected - closestAccessiblePoint).SizeSquared2D();
	bool closeEnough = FMath::Square(useReachabilityRadius) > distanceSqr;

#if WITH_EDITOR && DRAW_DEBUG_PATH
	FColor cylinderColor = FColor::Red;
	if (closeEnough)
	{
		cylinderColor = FColor::Green;
	}
	FVector cylinderStart = closestAccessiblePoint + FVector::UpVector * 10.0f + FVector::UpVector * 100.0f;
	FVector cylinderEnd = closestAccessiblePoint - FVector::UpVector * 10.0f + FVector::UpVector * 100.0f;
	DrawDebugCylinder(world, cylinderStart, cylinderEnd, useReachabilityRadius, 12, cylinderColor, false, 0.5, 0, 2.0f);
	TArray<FNavPathPoint> pathPoints = navMeshPath->GetPathPoints();
	DrawDebugDirectionalArrow(world, pathPoints[pathPoints.Num() - 2], closestAccessiblePoint, 100.0f, FColor::Orange, false, 0.5f, 0, 2.0f);
#endif

	return closeEnough;
}

bool UZombieSiegeUtils::DoesPathHaveHighCostAreas(UNavigationPath* path)
{
	return false;
}



bool UZombieSiegeUtils::AreEnemies(AActor* unitA, AActor* unitB)
{
	IGenericTeamAgentInterface* agentA = Cast<IGenericTeamAgentInterface>(unitA);
	IGenericTeamAgentInterface* agentB = Cast<IGenericTeamAgentInterface>(unitB);

	check(agentA);
	check(agentB);

	return FGenericTeamId::GetAttitude(agentA->GetGenericTeamId(), agentB->GetGenericTeamId()) == ETeamAttitude::Hostile;
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

bool UZombieSiegeUtils::ProjectLocationToNavMesh(UWorld* world, const FVector& inLocation, FVector& outNavLocation, const FVector& extent)
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(world);
	check(NavSys);
	ARecastNavMesh* NavigationData = Cast<ARecastNavMesh>(NavSys->MainNavData.Get());
	check(NavigationData);

	FNavLocation navLocation;
	bool bOk = NavSys->ProjectPointToNavigation(inLocation, navLocation, extent, NavigationData);
	outNavLocation = navLocation.Location;
	return bOk;
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
