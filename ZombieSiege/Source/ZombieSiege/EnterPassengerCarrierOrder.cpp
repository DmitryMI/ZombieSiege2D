// Fill out your copyright notice in the Description page of Project Settings.


#include "EnterPassengerCarrierOrder.h"

AUnitBase* UEnterPassengerCarrierOrder::GetTargetUnit()
{
	if (GetOrderState() == EOrderState::Executing)
	{
		targetCarrier = Cast<AUnitBase>(GetBlackboard()->GetValueAsObject("TargetUnit"));
	}

	return targetCarrier;
}

void UEnterPassengerCarrierOrder::SetTargetUnit(AUnitBase* targetUnit)
{
	targetCarrier = targetUnit;

	if (GetOrderState() == EOrderState::Executing)
	{
		GetBlackboard()->SetValueAsObject("TargetUnit", targetCarrier);
	}
}

void UEnterPassengerCarrierOrder::ExecuteOrder()
{
	Super::ExecuteOrder();

	SetTargetUnit(targetCarrier);
}
