// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieSiegeUtils.h"
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
