// Fill out your copyright notice in the Description page of Project Settings.


#include "EnumUtils.h"
#include "Kismet/GameplayStatics.h"

EFaceDirection UEnumUtils::GetNextFaceDirection(EFaceDirection direction)
{
	switch (direction)
	{
	case EFaceDirection::Up:
		return EFaceDirection::Right;
	case EFaceDirection::Right:
		return EFaceDirection::Down;
	case EFaceDirection::Down:
		return EFaceDirection::Left;
	default:
		return EFaceDirection::Up;
	}
}

EFaceDirection UEnumUtils::GetPreviousFaceDirection(EFaceDirection direction)
{
	switch (direction)
	{
	case EFaceDirection::Up:
		return EFaceDirection::Left;
	case EFaceDirection::Left:
		return EFaceDirection::Down;
	case EFaceDirection::Down:
		return EFaceDirection::Right;
	default:
		return EFaceDirection::Up;
	}
}

EUnitState UEnumUtils::GetNextUnitState(EUnitState state)
{
	int minState = static_cast<int>(EUnitState::None);
	int maxState = static_cast<int>(EUnitState::UNIT_STATE_MAX) - 1;

	int stateInt = static_cast<int>(state);

	stateInt++;

	if (stateInt > maxState)
	{
		stateInt = minState;
	}

	return static_cast<EUnitState>(stateInt);
}

EUnitState UEnumUtils::GetPreviousUnitState(EUnitState state)
{
	int minState = static_cast<int>(EUnitState::None);
	int maxState = static_cast<int>(EUnitState::UNIT_STATE_MAX) - 1;

	int stateInt = static_cast<int>(state);

	stateInt--;

	if (stateInt < 0)
	{
		stateInt = maxState;
	}

	return static_cast<EUnitState>(stateInt);
}

FString UEnumUtils::GetUnitStateName(EUnitState state)
{
	switch (state)
	{
	case EUnitState::None:
		return "None";
	case EUnitState::Moving:
		return "Moving";
	case EUnitState::Dying:
		return "Dying";
	case EUnitState::Birth:
		return "Birth";
	case EUnitState::Dead:
		return "Dead";
	case EUnitState::AttackingRelaxation:
		return "AttackingRelaxation";
	case EUnitState::AttackingBackswing:
		return "AttackingBackswing";
	default:
		checkNoEntry();
		return "ERROR";
	}
}

FString UEnumUtils::GetFaceDirectionName(EFaceDirection direction)
{
	switch (direction)
	{
	case EFaceDirection::Up:
		return "Up";
	case EFaceDirection::Left:
		return "Left";
	case EFaceDirection::Right:
		return "Right";
	case EFaceDirection::Down:
		return "Down";
	default:
		return "Multiple";
	}
}

FString UEnumUtils::GetOrderResultName(EOrderResult value)
{
	switch (value)
	{
	case EOrderResult::Success:
		return "Success";
	case EOrderResult::Fail:
		return "Fail";
	case EOrderResult::Abort:
		return "Abort";
	default:
		checkNoEntry();
	}

	return "ERROR";
}

