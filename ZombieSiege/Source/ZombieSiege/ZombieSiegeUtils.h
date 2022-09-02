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
};
