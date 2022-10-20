// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretCarrierControllerComponent.h"
#include "UnitAiController.h"
#include "TurretAiController.h"
#include "Turret.h"
#include "UnitOrder.h"
#include "AttackUnitOrder.h"
#include "ZombieSiegeUtils.h"

// Sets default values for this component's properties
UTurretCarrierControllerComponent::UTurretCarrierControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UTurretCarrierControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	AUnitAiController* controller = GetOwningController<AUnitAiController>();
	check(controller);

	controller->OnPossessedPawnChanged().AddUObject(this, &UTurretCarrierControllerComponent::OnOwnerPossessedPawnChangedHandler);
	controller->OnOrderExecutionStarted().AddUObject(this, &UTurretCarrierControllerComponent::OnCarrierOrderExecutionStartedHandler);
}

const TArray<ATurret*> UTurretCarrierControllerComponent::GetTurrets() const
{
	ITurretCarrier* carrier = GetOwningUnit<ITurretCarrier>();
	check(carrier);

	const TArray<ATurret*>& turrets = carrier->GetTurrets();
	return turrets;
}

void UTurretCarrierControllerComponent::UpdateTurretControllers()
{
	ITurretCarrier* carrier = GetOwningUnit<ITurretCarrier>();
	check(carrier);

	const TArray<ATurret*>& turrets = carrier->GetTurrets();

	if (turretAimStrategy == ETurretAimStrategy::Auto)
	{
		SetAllTurretsAutoAimEnabled(true);
	}
	else if (turretAimStrategy == ETurretAimStrategy::SpreadWhenNoOrders)
	{
		SetAllTurretsAutoAimEnabled(true);
	}
	else
	{
		SetAllTurretsAutoAimEnabled(false);
	}
}

void UTurretCarrierControllerComponent::OnOwnerPossessedPawnChangedHandler(const FPossessedPawnChangedEventArgs& args)
{
	
}

void UTurretCarrierControllerComponent::OnCarrierOrderExecutionStartedHandler(const FOrderExecutionStartedEventArgs& args)
{
	ITurretCarrier* carrier = GetOwningUnit<ITurretCarrier>();
	check(carrier);

	UUnitOrder* order = args.order;
	UAttackUnitOrder* attackUnitOrder = Cast<UAttackUnitOrder>(order);

	if (attackUnitOrder)
	{
		focusTarget = attackUnitOrder->GetTargetUnit();
		if (turretAimStrategy == ETurretAimStrategy::FocusOneTargetAlways || turretAimStrategy == ETurretAimStrategy::SpreadWhenNoOrders)
		{
			for (ATurret* turret : GetTurrets())
			{
				SetTurretTargetUnit(turret, focusTarget);
			}
		}
	}
	else // Any other order that does not specify TargetUnit
	{
		
	}
}

void UTurretCarrierControllerComponent::SetAllTurretsAutoAimEnabled(bool bEnabled)
{
	const TArray<ATurret*>& turrets = GetTurrets();

	for (ATurret* turret : turrets)
	{
		SetTurretAutoAimEnabled(turret, bEnabled);
	}
}

AUnitBase* UTurretCarrierControllerComponent::GetTurretTargetUnit(ATurret* turret)
{
	ATurretAIController* turretController = turret->GetController<ATurretAIController>();
	return Cast<AUnitBase>(turretController->GetTargetUnit());
}

void UTurretCarrierControllerComponent::SetTurretTargetUnit(ATurret* turret, AUnitBase* unit)
{
	ATurretAIController* turretController = turret->GetController<ATurretAIController>();

	if (turret->CanAttackTarget(unit, FAttackTestParameters(false, true, true, false)))
	{
		turretController->SetAutoAimEnabled(false);
		turretController->SetTargetUnit(unit);
	}
	else
	{
		turretController->SetAutoAimEnabled(true);
	}
	
}

AUnitBase* UTurretCarrierControllerComponent::GetClosestAliveAttackableEnemy()
{
	AUnitBase* controlledUnit = GetOwningUnit<AUnitBase>();
	AUnitAiController* controller = GetOwningController<AUnitAiController>();

	TArray<AUnitBase*> enemies;
	bool ok = controller->GetPerceivedAttackableEnemies(enemies);

	AUnitBase* closestEnemy = UZombieSiegeUtils::GetClosestUnit(enemies, controlledUnit->GetActorLocation());
	return closestEnemy;
}

void UTurretCarrierControllerComponent::SpreadTargetsAcrossTurrets()
{
	AUnitBase* owner = GetOwningUnit<AUnitBase>();
	AUnitAiController* controller = GetOwningController<AUnitAiController>();

	TArray<AUnitBase*> enemies;
	bool ok = controller->GetPerceivedAttackableEnemies(enemies);

	if (enemies.IsEmpty())
	{
		return;
	}

	auto& turrets = GetTurrets();

	for (int i = 0; i < turrets.Num(); i++)
	{
		ATurret* turret = turrets[i];
		AUnitBase* currentTarget = GetTurretTargetUnit(turret);
		if (currentTarget && turret->CanAttackTarget(currentTarget, FAttackTestParameters(false, true, true, false)))
		{
			// Do not reaim turrets
			continue;
		}

		int targetIndex = i % enemies.Num();
		currentTarget = enemies[targetIndex];
		SetTurretTargetUnit(turret, currentTarget);
	}
}

void UTurretCarrierControllerComponent::SetTurretAutoAimEnabled(ATurret* turret, bool bEnabled)
{
	ATurretAIController* turretController = turret->GetController<ATurretAIController>();
	turretController->SetAutoAimEnabled(bEnabled);
}


// Called every frame
void UTurretCarrierControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AUnitBase* owner = GetOwningUnit<AUnitBase>();

	if (focusTarget != nullptr && !owner->CanAttackTarget(focusTarget, FAttackTestParameters(false, true, true, false)))
	{
		focusTarget = nullptr;
	}

	switch (turretAimStrategy)
	{
	case ETurretAimStrategy::Auto:
		break;
	case ETurretAimStrategy::FocusOneTargetAlways:
		if (focusTarget == nullptr)
		{
			focusTarget = GetClosestAliveAttackableEnemy();
			if (focusTarget)
			{
				for (ATurret* turret : GetTurrets())
				{
					SetTurretTargetUnit(turret, focusTarget);
				}
			}
			else
			{
				SetAllTurretsAutoAimEnabled(false);
			}
		}
	case ETurretAimStrategy::SpreadWhenNoOrders:
		if (focusTarget == nullptr)
		{
			SpreadTargetsAcrossTurrets();
		}
		
		break;
	case ETurretAimStrategy::SpreadAlways:
		SpreadTargetsAcrossTurrets();
		break;
	}
}

