// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackOnMoveOrder.h"
#include "ZombieSiegeUtils.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h" 
#include "Perception/AISenseConfig_Touch.h" 


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

void UAttackOnMoveOrder::UnitPerceptionUpdated(AUnitBase* unit, FAIStimulus Stimulus)
{
	AUnitBase* controlledUnit = Cast<AUnitBase>(unitController->GetPawn());
	check(controlledUnit);
	if (!UZombieSiegeUtils::AreEnemies(controlledUnit, unit))
	{
		return;
	}

	AUnitAiController* controller = GetController();
	UAIPerceptionComponent* perceptionComponent = controller->GetAIPerceptionComponent();
	check(perceptionComponent);

	UAISenseConfig* senseConfig = perceptionComponent->GetSenseConfig(Stimulus.Type);
	if (senseConfig->IsA(UAISenseConfig_Sight::StaticClass()))
	{
		AUnitBase* currentTarget = GetTargetUnit();

		bool canAttackCurrentTargetImmidiately = currentTarget && controlledUnit->CanAttackTarget(currentTarget, FAttackTestParameters(EAttackTestFlags::Affiliation | EAttackTestFlags::Range));
		bool canAttackCurrentTargetLater = currentTarget && controlledUnit->CanAttackTarget(currentTarget, FAttackTestParameters(EAttackTestFlags::Affiliation));
		bool canAttackSensedUnitImmidiately = controlledUnit->CanAttackTarget(unit, FAttackTestParameters(EAttackTestFlags::Affiliation | EAttackTestFlags::Range));
		bool canAttackSensedUnitLater = controlledUnit->CanAttackTarget(unit, FAttackTestParameters(EAttackTestFlags::Affiliation));

		if (!canAttackCurrentTargetLater)
		{
			SetTargetUnit(unit);
		}
		else if (!canAttackCurrentTargetImmidiately)
		{
			if (canAttackSensedUnitImmidiately)
			{
				SetTargetUnit(unit);
			}
			else if (canAttackSensedUnitLater)
			{
				// TODO Use Nav distance, not linear distance
				float currentTargetDeltaSqr = (controlledUnit->GetActorLocation() - currentTarget->GetActorLocation()).SizeSquared2D();
				float sensedUnitDeltaSqr = (controlledUnit->GetActorLocation() - unit->GetActorLocation()).SizeSquared2D();
				if (sensedUnitDeltaSqr < currentTargetDeltaSqr)
				{
					SetTargetUnit(unit);
				}
			}
		}

	}
	else if (senseConfig->IsA(UAISenseConfig_Touch::StaticClass()))
	{
		if (controlledUnit->CanAttackTarget(unit, FAttackTestParameters(false, true, true, true)))
		{
			SetTargetUnit(unit);
		}
	}

}

void UAttackOnMoveOrder::ControlledUnitAttacked(const FDamageReceivedEventArgs& damageEventArgs)
{
	AUnitBase* controlledUnit = Cast<AUnitBase>(unitController->GetPawn());
	check(controlledUnit);

	AUnitBase* attacker = damageEventArgs.source;
	if (controlledUnit != damageEventArgs.target)
	{
		return;
	}

	if (!UZombieSiegeUtils::AreEnemies(controlledUnit, attacker))
	{
		return;
	}

	FAttackTestParameters currentTargetTestParams(EAttackTestFlags::Range);
	AUnitBase* currentTarget = GetTargetUnit();

	if (!currentTarget)
	{
		// No target at all -> Now the aggressor is the target
		SetTargetUnit(attacker);
		return;
	}

	if (controlledUnit->CanAttackTarget(currentTarget, currentTargetTestParams))
	{
		// We have a target and can attack it, do not change it
		return;
	}

	// We have a target, but the we cannot attack it right now -> Now the aggressor is the target
	SetTargetUnit(attacker);
}