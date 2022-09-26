// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitHudWidget.h"
#include "Building.h"

bool UUnitHudWidget::ShouldShowBuildingProgress()
{
	if (!unit)
	{
		return false;
	}

	ABuilding* building = Cast<ABuilding>(unit);
	if (!building)
	{
		return false;
	}

	return !building->IsFullyBuilt();
}

float UUnitHudWidget::GetBuildingProgressFraction()
{
	if (!unit)
	{
		return 0.0f;
	}

	ABuilding* building = Cast<ABuilding>(unit);
	if (!building)
	{
		return 0.0f;
	}

	return building->GetBuildingProgressFraction();
}

void UUnitHudWidget::SetUnit(AUnitBase* value)
{
	unit = value;
}

AUnitBase* UUnitHudWidget::GetUnit()
{
	return unit;
}
