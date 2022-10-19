// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EUnitOrderType : uint8
{
	None,
	HoldPosition,
	Move,
	AttackOnMove,
	AttackUnit,
	AttackPoint,
	Wander,
	Repair,
	Build,
	Gather
};