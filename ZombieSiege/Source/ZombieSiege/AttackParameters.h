#pragma once

#include "AttackParameters.generated.h"

USTRUCT(Blueprintable)
struct FAttackParameters
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector projectileSpawnRelativeLocation = FVector::Zero();

	UPROPERTY(BlueprintReadWrite)
	FVector projectileInitialLocalSpaceVelocity = FVector::ForwardVector;
};
