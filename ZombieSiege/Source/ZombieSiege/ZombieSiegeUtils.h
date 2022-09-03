// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UnitBase.h"
#include "ZombieSiegeUtils.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API UZombieSiegeUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "ZombieSiegeUtils", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "ignoredActors"))
	static AUnitBase* FindClosestAliveUnitInRadius(
		const UObject* WorldContextObject,
		const FVector& center,
		float searchRadius,
		ECollisionChannel collisionChannel,
		TSubclassOf<AUnitBase> unitClass,
		const TArray<AActor*>& ignoredActors);
	
	UFUNCTION(BlueprintCallable, Category = "ZombieSiegeUtils", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "ignoredActors"))
	static TArray<AUnitBase*> FindAliveUnitsInRadius(
		const UObject* WorldContextObject,
		const FVector& center,
		float searchRadius,
		ECollisionChannel collisionChannel,
		TSubclassOf<AUnitBase> unitClass,
		const TArray<AActor*>& ignoredActors);

	UFUNCTION(BlueprintCallable, Category = "ZombieSiegeUtils")
	static float GetDistance2DBetweenSimpleCollisions(AActor* actor1, AActor* actor2);

	UFUNCTION(BlueprintCallable, Category = "ZombieSiegeUtils", meta = (WorldContext = "WorldContextObject"))
	static bool IsLocationReachable(const UObject* WorldContextObject, AUnitBase* unit, FVector Position, float tolerance);
	UFUNCTION(BlueprintCallable, Category = "ZombieSiegeUtils")
	static bool IsLocationReachableWorld(UWorld* world, AUnitBase* unit, FVector Position, float tolerance);

	UFUNCTION(BlueprintCallable, Category = "ZombieSiegeUtils", meta = (WorldContext = "WorldContextObject"))
	static UNavigationPath* FindPathBetweenLocations(const UObject* WorldContextObject, FVector from, FVector to, AActor* pathFindingContext = nullptr);
	UFUNCTION(BlueprintCallable, Category = "ZombieSiegeUtils")
	static UNavigationPath* FindPathBetweenLocationsWorld(UWorld* world, FVector from, FVector to, AActor* pathFindingContext = nullptr);

	UFUNCTION(BlueprintCallable, Category = "ZombieSiegeUtils")
	static bool IsUnitOnNavMeshWorld(UWorld* world, AUnitBase* unit);

	UFUNCTION(BlueprintCallable, Category = "ZombieSiegeUtils")
	static bool GetFirstPointCloseToGoal(FVector goalLocation, float reachabilityRadius, UNavigationPath* navPath, FVector& result);

	UFUNCTION(BlueprintCallable, Category = "ZombieSiegeUtils", meta = (WorldContext = "WorldContextObject"))
	static bool GetBestLocationNearUnitToArrive(
		const UObject* WorldContextObject,
		AUnitBase* movingAgent,
		AUnitBase* targetAgent,
		float tolerance,
		FVector& OutLocation);

	UFUNCTION(BlueprintCallable, Category = "ZombieSiegeUtils")
	static bool GetBestLocationNearUnitToArriveWorld(
		UWorld* world,
		AUnitBase* movingAgent,
		AUnitBase* goalAgent,
		float tolerance,
		FVector& OutLocation);
};
