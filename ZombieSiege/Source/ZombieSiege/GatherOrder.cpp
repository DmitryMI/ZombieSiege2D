// Fill out your copyright notice in the Description page of Project Settings.


#include "GatherOrder.h"

UGatherOrder::UGatherOrder()
{
	orderType = EUnitOrderType::Gather;
}

void UGatherOrder::ExecuteOrder()
{
	Super::ExecuteOrder();

	UBlackboardComponent* blackboard = GetBlackboard();

	check(blackboard);

	blackboard->SetValueAsObject("GatheringTarget", gatherTarget);
}

void UGatherOrder::SetGatherTarget(ADoodad* target)
{
	check(target);
	gatherTarget = target;
}
