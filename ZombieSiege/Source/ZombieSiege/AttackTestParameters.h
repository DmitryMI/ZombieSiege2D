#pragma once

#include "AttackTestParameters.generated.h"

USTRUCT(BlueprintType)
struct FAttackTestParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bTestCooldown;

	UPROPERTY(EditAnywhere)
	bool bTestRange;

	UPROPERTY(EditAnywhere)
	bool bTestAffilation;

	UPROPERTY(EditAnywhere)
	bool bTestPhysicalState;

	FAttackTestParameters(bool bCooldown, bool bRange, bool bAffiliation, bool bPhysicalState)
	{
		bTestCooldown = bCooldown;
		bTestRange = bRange;
		bTestAffilation = bAffiliation;
		bTestPhysicalState = bPhysicalState;
	}
	
	FAttackTestParameters(bool bCooldown, bool bRange, bool bAffilation)
	{
		bTestCooldown = bCooldown;
		bTestRange = bRange;
		bTestAffilation = bAffilation;
		
		bTestPhysicalState = true;
	}
	
	FAttackTestParameters()
	{
		bTestCooldown = true;
		bTestRange = true;
		bTestAffilation = true;
		
		bTestPhysicalState = true;
	}	
};
