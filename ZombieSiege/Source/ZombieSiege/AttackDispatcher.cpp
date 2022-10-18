// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackDispatcher.h"

UAttackDispatcher::UAttackDispatcher()
{
	attackTimerDelegate.BindUObject(this, &UAttackDispatcher::OnAttackTimerElapsed);
}

UAttackDispatcher::~UAttackDispatcher()
{
	ClearTimer();
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
	return attackState == EAttackState::Backswing || attackState == EAttackState::Cooldown;
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
		bool attackHappened = CommitAttack();

		if (attackHappened)
		{
			SetTimer(weaponInfo->GetRelaxationDuration());
			attackState = EAttackState::Relaxation;
		}
		else
		{
			ClearTimer();
			attackState = EAttackState::None;
		}

		break;
	case EAttackState::Relaxation:

		SetTimer(weaponInfo->GetCooldownDuration());

		attackState = EAttackState::Cooldown;
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
	if (attackState == EAttackState::Backswing || attackState == EAttackState::Cooldown)
	{
		return false;
	}

	attackState = EAttackState::None;
	ClearTimer();

	TickStateMachine();

	return true;
}

bool UAttackDispatcher::CommitAttack()
{
	if (attackMode == EAttackMode::Target)
	{
		if (!weaponInfo->CanAttackTarget(owningUnit, targetUnit))
		{
			return false;
		}

		weaponInfo->AttackTarget(owningUnit, targetUnit);
	}
	else if (attackMode == EAttackMode::Point)
	{
		if (!weaponInfo->CanAttackPoint(owningUnit, targetPoint))
		{
			return false;
		}

		weaponInfo->AttackPoint(owningUnit, targetPoint);
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
	GetWorld()->GetTimerManager().SetTimer(attackTimerHandle, attackTimerDelegate, timeout, false, timeout);
}

void UAttackDispatcher::ClearTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(attackTimerHandle);
}

bool UAttackDispatcher::BeginAttackTarget(AUnitBase* owner, UWeaponInfo* weapon, AUnitBase* unit)
{
	check(owner);
	check(weapon);
	check(unit);

	if (!weapon->CanAttackTarget(owner, unit))
	{
		return false;
	}

	attackMode = EAttackMode::Target;
	targetUnit = unit;
	owningUnit = owner;
	weaponInfo = weapon;
	
	return BeginAttack();
}

bool UAttackDispatcher::BegineAttackPoint(AUnitBase* owner, UWeaponInfo* weapon, const FVector& point)
{
	check(owner);
	check(weapon);

	if (!weapon->CanAttackPoint(owner, point))
	{
		return false;
	}

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
