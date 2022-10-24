// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveOrder.h"

void UMoveOrder::ExecuteOrder()
{
	Super::ExecuteOrder();

	SetTargetLocation(targetLocation);
}

FVector UMoveOrder::GetTargetLocation()
{
	if (GetOrderState() == EOrderState::Executing)
	{
		targetLocation = GetBlackboard()->GetValueAsVector("TargetLocation");
	}

	return targetLocation;
}

void UMoveOrder::SetTargetLocation(const FVector& location)
{
	targetLocation = location;

	if (GetOrderState() == EOrderState::Executing)
	{
		GetBlackboard()->SetValueAsVector("TargetLocation", targetLocation);
	}
}
