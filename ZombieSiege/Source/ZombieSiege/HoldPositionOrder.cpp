// Fill out your copyright notice in the Description page of Project Settings.


#include "HoldPositionOrder.h"
#include "ZombieSiegeUtils.h"

UHoldPositionOrder::UHoldPositionOrder()
{
	orderType = EUnitOrderType::HoldPosition;
}

void UHoldPositionOrder::Execute()
{
	Super::Execute();

	GetBlackboard()->SetValueAsVector("TargetLocation", holdLocation);

	TArray<AUnitBase*> enemies;
	unitController->GetPerceivedAttackableEnemies(enemies);
	AUnitBase* closestEnemy = UZombieSiegeUtils::GetClosestUnit(enemies, unitController->GetPawn()->GetActorLocation());
	SetTargetUnit(closestEnemy);
}

AUnitBase* UHoldPositionOrder::GetTargetUnit()
{
	return Cast<AUnitBase>(GetBlackboard()->GetValueAsObject("TargetUnit"));
}

void UHoldPositionOrder::SetTargetUnit(AUnitBase* unit)
{
	GetBlackboard()->SetValueAsObject("TargetUnit", unit);
}

void UHoldPositionOrder::SetHoldLocation(const FVector& location)
{
	holdLocation = location;
}

void UHoldPositionOrder::TargetPerceptionStarted(AUnitBase* unit)
{
	AUnitBase* controlledUnit = Cast<AUnitBase>(unitController->GetPawn());
	check(controlledUnit);
	if (!UZombieSiegeUtils::AreEnemies(controlledUnit, unit))
	{
		return;
	}

	AUnitBase* currentTarget = GetTargetUnit();

	if (!currentTarget || controlledUnit->CanAttackTarget(currentTarget, FAttackTestParameters(false, false, true, false)))
	{
		if (controlledUnit->CanAttackTarget(unit, FAttackTestParameters(false, false, true, false)))
		{
			SetTargetUnit(unit);
		}
	}
}
