// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitManager.h"

ACTOR_SINGLETON_IMPLEMENTATION(AUnitManager)

// Sets default values
AUnitManager::AUnitManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AUnitManager::BeginPlay()
{
	Super::BeginPlay();
	
	InitActorSingleton();
}

TMap<FName, TSubclassOf<AUnitBase>>& AUnitManager::GetUnitClassMapMutable()
{
	return unitClassMap;
}

const TMap<FName, TSubclassOf<AUnitBase>>& AUnitManager::GetUnitClassMap() const
{
	return unitClassMap;
}

TSubclassOf<AUnitBase> AUnitManager::GetUnitClass(const FName& name) const
{
	if (!HasUnitClass(name))
	{
		FString nameStr = name.ToString();
		UE_LOG(LogTemp, Error, TEXT("UnitClass with name %s does not exist!"), *nameStr);
		return nullptr;
	}

	return unitClassMap[name];
}

bool AUnitManager::AddUnitClass(const FName& name, TSubclassOf<AUnitBase> clazz)
{
	if (HasUnitClass(name))
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to add an existing UnitClass (%s)!"), *name.ToString());
		return false;
	}

	check(clazz);
	unitClassMap.Add(name, clazz);
	return true;
}

bool AUnitManager::HasUnitClass(const FName& name) const
{
	return unitClassMap.Contains(name);
}

