// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAnimationState : uint8
{
	None,
	Moving,
	Attacking,
	Dying,
};