// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"

void ABuilding::BeginPlay()
{
	if (bIsBuiltOnSpawn)
	{
		SetBuildingProgress(GetMaxHealth());
	}
	else
	{
		SetUnitState(EUnitState::Birth);
		SetBuildingProgress(0);
	}
}

ABuilding::ABuilding()
{
	AddClassifications(EUnitClassification::Building);
}

float ABuilding::ReceiveHealing(const FHealingInstance& repair)
{
	float healingActual = Super::ReceiveHealing(repair);

	if (!IsFullyBuilt())
	{
		AddBuildingProgress(healingActual);
	}

	return healingActual;
}

float ABuilding::GetBuildingProgressFraction()
{
	return buildingProgress / GetMaxHealth();
}

float ABuilding::GetBuildingProgress()
{
	return buildingProgress;
}

void ABuilding::SetBuildingProgressFraction(float progressFraction)
{
	progressFraction = FMath::Clamp(progressFraction, 0.0f, 1.0f);

	SetBuildingProgress(progressFraction * GetMaxHealth());
}

void ABuilding::SetBuildingProgress(float progress)
{
	progress = FMath::Clamp(progress, 0, GetMaxHealth());
	buildingProgress = progress;

	if (GetUnitState() == EUnitState::Birth && IsFullyBuilt())
	{
		buildingProgress = GetMaxHealth();
		SetUnitState(EUnitState::None);
	}
	else if (GetUnitState() != EUnitState::Birth && buildingProgress < GetMaxHealth())
	{
		UE_LOG(LogTemp, Fatal, TEXT("SetBuildingProgress decreased building progress of a fully-built building! This is not supported."));
	}
}

void ABuilding::AddBuildingProgress(float addProgress)
{
	check(addProgress >= 0);
	float progress = GetBuildingProgress() + addProgress;

	SetBuildingProgress(progress);
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
