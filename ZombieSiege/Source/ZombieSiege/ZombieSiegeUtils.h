// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UnitBase.h"
#include "FaceDirection.h"
#include "ZombieSiegeUtils.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API UZombieSiegeUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	template<typename T>
	static bool SetObjectProperty(UObject* obj, const FName& propName, const T& value)
	{
		FProperty* Property = obj->GetClass()->FindPropertyByName(propName);
		if (Property)
		{
			T* ptrValue = Property->ContainerPtrToValuePtr<T>(obj);
			if (ptrValue)
			{
				*ptrValue = value;
				return true;
			}
		}
		return false;
	}

	template<typename T>
	static bool GetObjectProperty(UObject* obj, const FName& propName, T& outValue)
	{
		FProperty* Property = obj->GetClass()->FindPropertyByName(propName);
		if (Property)
		{
			T* ptrValue = Property->ContainerPtrToValuePtr<T>(obj);
			if (ptrValue)
			{
				outValue = *ptrValue;
				return true;
			}
		}
		return false;
	}

	template<typename T>
	static bool CompareArrays(const TArray<T>& a, const TArray<T>& b)
	{
		if (a.Num() != b.Num())
		{
			return false;
		}

		for (int i = 0; i < a.Num(); i++)
		{
			if (a[i] != b[i])
			{
				return false;
			}
		}

		return true;
	}


	/// <summary>
	/// Inserts an item to an array keeping the ascending sort order.
	/// </summary>
	/// <typeparam name="T">Array item type</typeparam>
	/// <typeparam name="Comparator">Callable comparator typeparam with signature (T a, T b) -> int.</typeparam>
	/// <param name="arr">Array to insert to</param>
	/// <param name="item">Inserted item</param>
	/// <param name="comparator">Comparator object</param>
	template<typename T, typename Comparator>
	static void InsertSortedAscending(TArray<T>& arr, const T& item, const Comparator& comparator)
	{
		int index = 0;
		for (index; index < arr.Num(); index++)
		{
			if (comparator(item, arr[index]) < 0)
			{
				break;
			}
		}

		if (index == arr.Num())
		{
			arr.Add(item);
		}
		else
		{
			arr.Insert(item, index);
		}
	}

	template<typename T>
	static int DefaultComparator(T a, T b)
	{
		if (a < b)
		{
			return -1;
		}
		else if (a > b)
		{
			return 1;
		}

		return 0;
	}


	UFUNCTION(BlueprintCallable, Category = "ZombieSiegeUtils")
	static int CompareDistances2D(AActor* actorA, AActor* actorB, AActor* pivotActor);

	UFUNCTION(BlueprintCallable, Category = "ZombieSiegeUtils")
	static UObject* GetClassDefaultObject(UClass* clazz)
	{
		if (!clazz)
		{
			return nullptr;
		}
		UObject* obj = clazz->GetDefaultObject();

		return obj;
	}

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

	UFUNCTION(BlueprintCallable, Category = "ZombieSiegeUtils")
	static bool AreEnemies(AUnitBase* unitA, AUnitBase* unitB);

	UFUNCTION(BlueprintCallable, Category = "ZombieSiegeUtils")
	static int GetDirectionSpriteIndex(EFaceDirection faceDirectionEnum);
};