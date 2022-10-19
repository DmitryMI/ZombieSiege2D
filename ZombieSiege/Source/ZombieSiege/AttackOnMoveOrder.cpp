// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackOnMoveOrder.h"

UAttackOnMoveOrder::UAttackOnMoveOrder()
{
	orderType = EUnitOrderType::AttackOnMove;
}

void UAttackOnMoveOrder::Execute()
{
	Super::Execute();

	UBlackboardComponent* blackboard = GetBlackboard();
	check(blackboard);

	blackboard->SetValueAsVector("TargetLocation", targetLocation);
}

void UAttackOnMoveOrder::SetTargetLocation(const FVector& location)
{
	targetLocation = location;
}
