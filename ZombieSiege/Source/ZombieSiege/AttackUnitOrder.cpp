
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

void UAttackUnitOrder::Execute()
{
	Super::Execute();

	UBlackboardComponent* blackboard = GetBlackboard();
	check(blackboard);

	blackboard->SetValueAsObject("Target", targetUnit);
}
