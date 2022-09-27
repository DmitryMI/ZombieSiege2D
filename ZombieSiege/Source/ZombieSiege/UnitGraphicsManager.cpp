// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitGraphicsManager.h"

ACTOR_SINGLETON_IMPLEMENTATION(AUnitGraphicsManager)

// Sets default values
AUnitGraphicsManager::AUnitGraphicsManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AUnitGraphicsManager::BeginPlay()
{
	Super::BeginPlay();
	
	InitActorSingleton();

	for (auto unitGraphicsClassPair : unitGraphicsDataClasses)
	{
		FName name = unitGraphicsClassPair.Key;
		UUnitGraphicsData* unitGraphicsData = UUnitGraphicsData::CreateUnitGraphicsData(this, name, unitGraphicsClassPair.Value);
		unitGraphicsDataMap.Add(name, unitGraphicsData);
	}
}

UUnitGraphicsData* AUnitGraphicsManager::GetUnitGraphicsData(const FName& name) const
{
	if (!HasUnitGraphicsData(name))
	{
		FString nameStr = name.ToString();
		UE_LOG(LogTemp, Error, TEXT("UUnitGraphicsData with name %s does not exist!"), *nameStr);
		return nullptr;
	}

	return unitGraphicsDataMap[name];
}

bool AUnitGraphicsManager::AddUnitGraphicsData(const FName& name, UUnitGraphicsData* unitGraphicsData)
{
	if (HasUnitGraphicsData(name))
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to add an existing UnitGraphicsData (%s)!"), *name.ToString());
		return false;
	}

	check(unitGraphicsData);
	unitGraphicsDataMap.Add(name, unitGraphicsData);
	return true;
}

bool AUnitGraphicsManager::HasUnitGraphicsData(const FName& name) const
{
	return unitGraphicsDataMap.Contains(name);
}


