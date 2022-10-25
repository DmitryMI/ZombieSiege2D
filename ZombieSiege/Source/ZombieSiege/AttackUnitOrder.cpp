
// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackUnitOrder.h"

UAttackUnitOrder::UAttackUnitOrder()
{
	orderType = EUnitOrderType::AttackUnit;
}

void UAttackUnitOrder::SetTargetUnit(AUnitBase* unit)
{
	targetUnit = unit;

	if (GetOrderState() == EOrderState::Executing)
	{
		GetBlackboard()->SetValueAsObject("TargetUnit", targetUnit);
	}
}

AUnitBase* UAttackUnitOrder::GetTargetUnit()
{
	if (GetOrderState() == EOrderState::Executing)
	{
		targetUnit = Cast<AUnitBase>(GetBlackboard()->GetValueAsObject("TargetUnit"));
	}
	return targetUnit;
}

void UAttackUnitOrder::ExecuteOrder()
{
	Super::ExecuteOrder();

	SetTargetUnit(targetUnit);
}

FString UAttackUnitOrder::ToString()
{
	AUnitBase* unit = GetTargetUnit();
	FString targetUnitName = "NULL";
	if (unit)
	{
		unit->GetName(targetUnitName);
	}

	FString str = FString::Printf(TEXT("%s (TargetUnit: %s)"), *Super::ToString(), *targetUnitName);
	return str;
}
