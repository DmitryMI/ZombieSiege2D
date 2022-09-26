// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"

ABuilding::ABuilding()
{
	AddClassifications(EUnitClassification::Building);

	
}

void ABuilding::ReceiveRepair(const FHealingInstance& repair)
{
}

bool ABuilding::IsBuiltOnSpawn()
{
	return bIsBuiltOnSpawn;
}

void ABuilding::SetIsBuiltOnSpawn(bool isBuiltOnSpawn)
{
	bIsBuiltOnSpawn = isBuiltOnSpawn;
}
