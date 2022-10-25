// Fill out your copyright notice in the Description page of Project Settings.


#include "GatherOrder.h"

UGatherOrder::UGatherOrder()
{
	orderType = EUnitOrderType::Gather;
}

void UGatherOrder::ExecuteOrder()
{
	Super::ExecuteOrder();

	SetGatherTarget(gatherTarget);
}

void UGatherOrder::SetGatherTarget(ADoodad* target)
{
	check(target);
	gatherTarget = target;

	if (GetOrderState() == EOrderState::Executing)
	{
		GetBlackboard()->SetValueAsObject("GatheringTarget", gatherTarget);
	}
}

ADoodad* UGatherOrder::GetGatherTarget()
{
	if (GetOrderState() == EOrderState::Executing)
	{
		gatherTarget = Cast<ADoodad>(GetBlackboard()->GetValueAsObject("GatheringTarget"));
	}
	return gatherTarget;
}

FString UGatherOrder::ToString()
{
	FString doodadName = "NULL";
	ADoodad* doodad = GetGatherTarget();
	if (doodad)
	{
		doodadName = GetGatherTarget()->GetName();
	}
	FString str = FString::Printf(TEXT("%s(GatherTarget: %s)"), *Super::ToString(), *doodadName);
	return str;
}
