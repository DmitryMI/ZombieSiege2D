
// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackUnitOrder.h"

UAttackUnitOrder::UAttackUnitOrder()
{
	orderType = EUnitOrderType::AttackUnit;
}

void UAttackUnitOrder::SetTargetUnit(AUnitBase* unit)
{
	targetUnit = unit;
}

AUnitBase* UAttackUnitOrder::GetTargetUnit()
{
	return targetUnit;
}

void UAttackUnitOrder::ExecuteOrder()
{
	Super::ExecuteOrder();

	UBlackboardComponent* blackboard = GetBlackboard();
	check(blackboard);

	blackboard->SetValueAsObject("Target", targetUnit);
}
