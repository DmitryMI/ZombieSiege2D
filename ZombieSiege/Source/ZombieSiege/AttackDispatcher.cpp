// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackDispatcher.h"
#include "WeaponInfo.h"

UAttackDispatcher::UAttackDispatcher()
{
	attackTimerDelegate.BindUObject(this, &UAttackDispatcher::OnAttackTimerElapsed);
}

UAttackDispatcher::~UAttackDispatcher()
{
	attackTimerDelegate.Unbind();
}

EAttackMode UAttackDispatcher::GetAttackMode()
{
	return attackMode;
}

EAttackState UAttackDispatcher::GetAttackState()
{
	return attackState;
}

UWeaponInfo* UAttackDispatcher::GetWeaponInfo()
{
	return weaponInfo;
}

AUnitBase* UAttackDispatcher::GetOwningUnit()
{
	return owningUnit;
}

AUnitBase* UAttackDispatcher::GetTargetUnit()
{
	return targetUnit;
}

FVector UAttackDispatcher::GetTargetPoint()
{
	return targetPoint;
}

bool UAttackDispatcher::IsOnCooldown()
{
	return attackState != EAttackState::None;
}

bool UAttackDispatcher::CanBeginAttackTarget(AUnitBase* owner, UWeaponInfo* weapon, AUnitBase* unit)
{
	if (!weapon)
	{
		return false;
	}

	return !IsOnCooldown() && weapon->CanAttackTarget(owner, unit);
}

bool UAttackDispatcher::CanBeginAttackPoint(AUnitBase* owner, UWeaponInfo* weapon, const FVector& point)
{
	if (!weapon)
	{
		return false;
	}
	return !IsOnCooldown() && weapon->CanAttackPoint(owner, point);
}

void UAttackDispatcher::OnAttackTimerElapsed()
{
	TickStateMachine();
}

void UAttackDispatcher::TickStateMachine()
{
	EAttackState stateOld = attackState;

	switch (attackState)
	{
	case EAttackState::None:
		attackState = EAttackState::Backswing;
		SetTimer(weaponInfo->GetBackswingDuration());
		break;

	case EAttackState::Backswing:

		if (CommitAttack())
		{
			attackState = EAttackState::Relaxation;
			SetTimer(weaponInfo->GetRelaxationDuration());			
		}
		else
		{
			attackState = EAttackState::None;
			ClearTimer();
		}

		break;
	case EAttackState::Relaxation:
		attackState = EAttackState::Cooldown;
		SetTimer(weaponInfo->GetCooldownDuration());
		break;
	case EAttackState::Cooldown:
		attackState = EAttackState::None;
		break;
	}

	if (stateOld != attackState)
	{
		BroadcastStateChangedEvent(stateOld, attackState);
	}
}

bool UAttackDispatcher::BeginAttack()
{
	attackState = EAttackState::None;
	ClearTimer();

	TickStateMachine();

	return true;
}

bool UAttackDispatcher::CommitAttack()
{
	if (attackMode == EAttackMode::Target)
	{
		if (!owningUnit->CanFinishAttackTargetWithWeapon(targetUnit, weaponInfo))
		{
			return false;
		}

		weaponInfo->AttackTarget(owningUnit, targetUnit, attackParameters);
	}
	else if (attackMode == EAttackMode::Point)
	{
		if (!owningUnit->CanFinishAttackPointWithWeapon(targetPoint, weaponInfo))
		{
			return false;
		}

		weaponInfo->AttackPoint(owningUnit, targetPoint, attackParameters);
	}
	else
	{
		checkNoEntry();
	}

	return true;
}

void UAttackDispatcher::BroadcastStateChangedEvent(EAttackState stateOld, EAttackState stateNew)
{
	FAttackDispatcherStateChangedEventArgs args(this, stateOld, stateNew);
	onAttackDispatcherStateChangedEvent.Broadcast(args);
}

void UAttackDispatcher::SetTimer(float timeout)
{
	if (FMath::IsNearlyZero(timeout))
	{
		attackTimerDelegate.ExecuteIfBound();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(attackTimerHandle, attackTimerDelegate, timeout, false, timeout);
	}
}

void UAttackDispatcher::ClearTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(attackTimerHandle);
}

bool UAttackDispatcher::BeginAttackTarget(AUnitBase* owner, UWeaponInfo* weapon, AUnitBase* unit, const FAttackParameters& params)
{
	check(owner);
	check(weapon);
	check(unit);

	if (!CanBeginAttackTarget(owner, weapon, unit))
	{
		return false;
	}

	attackParameters = params;
	attackMode = EAttackMode::Target;
	targetUnit = unit;
	owningUnit = owner;
	weaponInfo = weapon;
	
	return BeginAttack();
}

bool UAttackDispatcher::BeginAttackPoint(AUnitBase* owner, UWeaponInfo* weapon, const FVector& point, const FAttackParameters& params)
{
	check(owner);
	check(weapon);

	if (!CanBeginAttackPoint(owner, weapon, point))
	{
		return false;
	}

	attackParameters = params;
	attackMode = EAttackMode::Point;
	targetUnit = nullptr;
	targetPoint = point;
	weaponInfo = weapon;

	return BeginAttack();
}

void UAttackDispatcher::CancelAttack()
{
	attackState = EAttackState::None;
	targetUnit = nullptr;
	ClearTimer();
}

FAttackDispatcherStateChangedEventArgs::FAttackDispatcherStateChangedEventArgs(UAttackDispatcher* dispatcherArg, EAttackState stateOldArg, EAttackState stateNewArg)
{
	dispatcher = dispatcherArg;
	stateOld = stateOldArg;
	stateNew = stateNewArg;
}

FAttackDispatcherStateChangedEventArgs::FAttackDispatcherStateChangedEventArgs()
{
	dispatcher = nullptr;
	stateOld = EAttackState::None;
	stateNew = EAttackState::None;
}
