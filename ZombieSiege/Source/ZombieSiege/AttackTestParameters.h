#pragma once

#include "Macros.h"
#include "AttackTestParameters.generated.h"


UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EAttackTestFlags : uint8
{
	NONE = 0 UMETA(Hidden),
	Cooldown =		1 << 0,
	Range =			1 << 1,
	Affiliation =	1 << 2,
	PhysicalState = 1 << 3,
	Reachability = 1 << 4,
};

ENUM_CLASS_FLAGS(EAttackTestFlags);


USTRUCT(BlueprintType)
struct FAttackTestParameters
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (Bitmask, BitmaskEnum = EAttackTestFlags))
	uint8 AttackTestFlags;

	GENERATE_FLAG_GETTER(AttackTestFlags, EAttackTestFlags, Cooldown);
	GENERATE_FLAG_SETTER(AttackTestFlags, EAttackTestFlags, Cooldown);

	GENERATE_FLAG_GETTER(AttackTestFlags, EAttackTestFlags, Range);
	GENERATE_FLAG_SETTER(AttackTestFlags, EAttackTestFlags, Range);

	GENERATE_FLAG_GETTER(AttackTestFlags, EAttackTestFlags, Affiliation);
	GENERATE_FLAG_SETTER(AttackTestFlags, EAttackTestFlags, Affiliation);

	GENERATE_FLAG_GETTER(AttackTestFlags, EAttackTestFlags, PhysicalState);
	GENERATE_FLAG_SETTER(AttackTestFlags, EAttackTestFlags, PhysicalState);

	GENERATE_FLAG_GETTER(AttackTestFlags, EAttackTestFlags, Reachability);
	GENERATE_FLAG_SETTER(AttackTestFlags, EAttackTestFlags, Reachability);

	FAttackTestParameters(EAttackTestFlags flags)
	{
		AttackTestFlags = static_cast<uint8>(flags);
	}

	FAttackTestParameters(bool bCooldown, bool bRange, bool bAffiliation, bool bPhysicalState)
	{
		SetCooldownFlag(bCooldown);
		SetRangeFlag(bRange);
		SetAffiliationFlag(bAffiliation);
		SetPhysicalStateFlag(bPhysicalState);
	}
	
	FAttackTestParameters(bool bCooldown, bool bRange, bool bAffiliation)
	{
		SetCooldownFlag(bCooldown);
		SetRangeFlag(bRange);
		SetAffiliationFlag(bAffiliation);
		SetPhysicalStateFlag(true);
	}
	
	FAttackTestParameters()
	{
		SetCooldownFlag(true);
		SetRangeFlag(true);
		SetAffiliationFlag(true);
		SetPhysicalStateFlag(true);
		SetReachabilityFlag(true);
	}	
};
