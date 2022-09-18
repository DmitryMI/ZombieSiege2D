// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EUnitClassification : uint8
{
	NONE = 0 UMETA(Hidden),
	Organism = 1 << 0,
	Building = 1 << 1,
	Mechanical = 1 << 2,
	DestructibleDoodad = 1 << 3,
	PassengerCarrier = 1 << 4,
};

ENUM_CLASS_FLAGS(EUnitClassification);