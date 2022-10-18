#pragma once

#include "AttackParameters.generated.h"

USTRUCT(Blueprintable)
struct FAttackParameters
{
	GENERATED_BODY()
	FVector projectileSpawnRelativeLocation = FVector::Zero();
};
