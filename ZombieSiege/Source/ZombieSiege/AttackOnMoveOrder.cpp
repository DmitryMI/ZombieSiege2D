// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackOnMoveOrder.h"
#include "ZombieSiegeUtils.h"


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

	TArray<AUnitBase*> enemies;
	unitController->GetPerceivedAttackableEnemies(enemies);
	AUnitBase* closestEnemy = UZombieSiegeUtils::GetClosestUnit(enemies, unitController->GetPawn()->GetActorLocation());
	SetTargetUnit(closestEnemy);
}

void UAttackOnMoveOrder::SetTargetLocation(const FVector& location)
{
	targetLocation = location;
}

AUnitBase* UAttackOnMoveOrder::GetTargetUnit()
{
	return Cast<AUnitBase>(GetBlackboard()->GetValueAsObject("TargetUnit"));
}

void UAttackOnMoveOrder::SetTargetUnit(AUnitBase* unit)
{
	GetBlackboard()->SetValueAsObject("TargetUnit", unit);
}

void UAttackOnMoveOrder::TargetPerceptionStarted(AUnitBase* unit)
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
