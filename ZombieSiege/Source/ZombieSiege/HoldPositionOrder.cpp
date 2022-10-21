// Fill out your copyright notice in the Description page of Project Settings.


#include "HoldPositionOrder.h"
#include "ZombieSiegeUtils.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h" 
#include "Perception/AISenseConfig_Touch.h" 

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

void UHoldPositionOrder::UnitPerceptionUpdated(AUnitBase* unit, FAIStimulus Stimulus)
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

		if (!currentTarget || controlledUnit->CanAttackTarget(currentTarget, FAttackTestParameters(false, false, true, false)))
		{
			if (controlledUnit->CanAttackTarget(unit, FAttackTestParameters(false, false, true, false)))
			{
				SetTargetUnit(unit);
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

void UHoldPositionOrder::ControlledUnitAttacked(const FDamageReceivedEventArgs& damageEventArgs)
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

	if(controlledUnit->CanAttackTarget(currentTarget, currentTargetTestParams))
	{
		// We have a target and can attack it, do not change it
		return;
	}

	// We have a target, but the we cannot attack it right now -> Now the aggressor is the target
	SetTargetUnit(attacker);
}

