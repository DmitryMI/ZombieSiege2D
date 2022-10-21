// Fill out your copyright notice in the Description page of Project Settings.


#include "RepairOrder.h"

URepairOrder::URepairOrder()
{
	orderType = EUnitOrderType::Repair;
}

void URepairOrder::ExecuteOrder()
{
	Super::ExecuteOrder();
	GetBlackboard()->SetValueAsObject("Building", targetBuilding);
}

void URepairOrder::SetTargetBuilding(ABuilding* building)
{
	targetBuilding = building;
}
