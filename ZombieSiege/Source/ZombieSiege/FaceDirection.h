// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EFaceDirection : uint8
{
	NONE = 0 UMETA(Hidden),
	Up = 1 << 0,
	Down = 1 << 1,
	Right = 1 << 2,
	Left = 1 << 3,
};

ENUM_CLASS_FLAGS(EFaceDirection);