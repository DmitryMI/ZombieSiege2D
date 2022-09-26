// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"

void ABuilding::BeginPlay()
{
	if (bIsBuiltOnSpawn)
	{
		buildingProgress = GetMaxHealth();
		SetUnitState(EUnitState::None);
	}
	else
	{
		buildingProgress = 0;
		SetUnitState(EUnitState::Birth);
	}
}

ABuilding::ABuilding()
{
	AddClassifications(EUnitClassification::Building);

	
}

float ABuilding::ReceiveHealing(const FHealingInstance& repair)
{
	float healingActual = Super::ReceiveHealing(repair);

	if (buildingProgress < GetMaxHealth())
	{
		buildingProgress += healingActual;
	}

	if (buildingProgress >= GetMaxHealth() && GetUnitState() == EUnitState::Birth)
	{
		buildingProgress = GetMaxHealth();
		SetUnitState(EUnitState::None);
	}

	return healingActual;
}

bool ABuilding::IsBuiltOnSpawn()
{
	return bIsBuiltOnSpawn;
}

bool ABuilding::IsFullyBuilt()
{
	return FMath::IsNearlyEqual(buildingProgress, GetMaxHealth());
}

void ABuilding::SetIsBuiltOnSpawn(bool isBuiltOnSpawn)
{
	bIsBuiltOnSpawn = isBuiltOnSpawn;
}
