// Fill out your copyright notice in the Description page of Project Settings.


#include "HoldPositionOrder.h"

UHoldPositionOrder::UHoldPositionOrder()
{
	orderType = EUnitOrderType::HoldPosition;
}

void UHoldPositionOrder::Execute()
{
	Super::Execute();

	GetBlackboard()->SetValueAsVector("TargetLocation", holdLocation);
}

void UHoldPositionOrder::SetHoldLocation(const FVector& location)
{
	holdLocation = location;
}
