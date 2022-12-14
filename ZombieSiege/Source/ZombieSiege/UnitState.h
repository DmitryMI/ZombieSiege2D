// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EUnitState : uint8
{
	None,	
	Moving,
	AttackingBackswing,
	AttackingRelaxation,
	Dying,
	Dead,
	Birth,
	UNIT_STATE_MAX
};