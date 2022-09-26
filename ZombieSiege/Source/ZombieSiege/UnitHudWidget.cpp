// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitHudWidget.h"

void UUnitHudWidget::SetUnit(AUnitBase* value)
{
	unit = value;
}

AUnitBase* UUnitHudWidget::GetUnit()
{
	return unit;
}
